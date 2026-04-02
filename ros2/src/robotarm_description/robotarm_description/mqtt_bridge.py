#!/usr/bin/env python3
"""
MQTT-zu-ROS2 Bridge fuer den Adeept 5-DOF Roboterarm.

Subscribt auf robotarm/imu via MQTT und published /joint_states fuer RViz2.
Die IMU-Euler-Winkel (3 Sensoren) werden auf die 5 Gelenke gemappt.

Unterstuetzt zwei Arme (front/back) mit Toggle via ROS2 Service.
"""

import json
import math
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import JointState
from std_srvs.srv import Trigger
import paho.mqtt.client as mqtt


class MqttBridge(Node):
    def __init__(self):
        super().__init__('mqtt_bridge')

        # Parameter
        self.declare_parameter('mqtt_host', 'localhost')
        self.declare_parameter('mqtt_port', 1883)
        self.declare_parameter('mqtt_user', 'dashboard_ws')
        self.declare_parameter('mqtt_pass', '')
        self.declare_parameter('mqtt_topic', 'robotarm/imu')
        self.declare_parameter('target_arm', 'front')
        self.declare_parameter('wall_mode', False)

        self.mqtt_host = self.get_parameter('mqtt_host').value
        self.mqtt_port = self.get_parameter('mqtt_port').value
        self.mqtt_user = self.get_parameter('mqtt_user').value
        self.mqtt_pass = self.get_parameter('mqtt_pass').value
        self.mqtt_topic = self.get_parameter('mqtt_topic').value
        self.target_arm = self.get_parameter('target_arm').value
        self.wall_mode = self.get_parameter('wall_mode').value

        # ROS Publisher
        self.joint_pub = self.create_publisher(JointState, '/joint_states', 10)

        # Joint-Namen abhaengig vom Modus
        if self.wall_mode:
            self.joint_names_front = [
                'base_joint_l', 'shoulder_joint_l', 'elbow_joint_l',
                'wrist_joint_l', 'gripper_joint_l',
            ]
            self.joint_names_back = [
                'base_joint_r', 'shoulder_joint_r', 'elbow_joint_r',
                'wrist_joint_r', 'gripper_joint_r',
            ]
        else:
            self.joint_names_front = [
                'base_joint', 'shoulder_joint', 'elbow_joint',
                'wrist_joint', 'gripper_left_joint',
            ]
            self.joint_names_back = self.joint_names_front

        # Toggle-Service
        self.toggle_srv = self.create_service(
            Trigger, 'toggle_arm', self.toggle_arm_callback)

        # MQTT Client
        self.mqtt_client = mqtt.Client(
            client_id='ros2_bridge',
            callback_api_version=mqtt.CallbackAPIVersion.VERSION2
        )
        if self.mqtt_user:
            self.mqtt_client.username_pw_set(self.mqtt_user, self.mqtt_pass)
        self.mqtt_client.on_connect = self.on_mqtt_connect
        self.mqtt_client.on_message = self.on_mqtt_message
        self.mqtt_client.on_disconnect = self.on_mqtt_disconnect

        # Verbindung starten
        self.get_logger().info(
            f'Verbinde mit MQTT {self.mqtt_host}:{self.mqtt_port}...')
        self.get_logger().info(
            f'Zielarm: {self.target_arm} (toggle via: ros2 service call /toggle_arm std_srvs/srv/Trigger)')
        try:
            self.mqtt_client.connect_async(self.mqtt_host, self.mqtt_port)
            self.mqtt_client.loop_start()
        except Exception as e:
            self.get_logger().error(f'MQTT-Verbindungsfehler: {e}')

        # Timer fuer Status-Log
        self.msg_count = 0
        self.create_timer(5.0, self.status_log)

    def toggle_arm_callback(self, request, response):
        if self.target_arm == 'front':
            self.target_arm = 'back'
        else:
            self.target_arm = 'front'
        response.success = True
        response.message = f'Zielarm: {self.target_arm}'
        self.get_logger().info(f'Arm gewechselt -> {self.target_arm}')
        return response

    @property
    def active_joint_names(self):
        if self.target_arm == 'back':
            return self.joint_names_back
        return self.joint_names_front

    def on_mqtt_connect(self, client, userdata, flags, rc, properties=None):
        if rc == 0:
            self.get_logger().info('MQTT verbunden!')
            client.subscribe(self.mqtt_topic)
        else:
            self.get_logger().error(f'MQTT-Verbindung fehlgeschlagen: rc={rc}')

    def on_mqtt_disconnect(self, client, userdata, flags, rc, properties=None):
        self.get_logger().warn('MQTT getrennt — Reconnect...')

    def on_mqtt_message(self, client, userdata, msg):
        try:
            data = json.loads(msg.payload.decode())
            self.imu_to_joints(data)
            self.msg_count += 1
        except (json.JSONDecodeError, KeyError) as e:
            self.get_logger().warn(f'Paket-Fehler: {e}')

    def imu_to_joints(self, data):
        """
        Mappt 3 IMU-Sensoren auf 5 Gelenke.

        Sensor-Zuordnung (aus esp32_controller):
          S0 = Oberarm-IMU   -> Shoulder + Elbow
          S1 = Unterarm-IMU  -> Elbow-Verfeinerung + Wrist
          S2 = Hand-IMU      -> Wrist + Gripper-Proxy

        IMU-Daten: h=heading (yaw), r=roll, p=pitch
        Alle Werte in Grad (0-360 fuer heading, +-180 fuer roll/pitch).
        """
        sensoren = data.get('s', [])
        if len(sensoren) < 3:
            return

        s0 = sensoren[0]  # Oberarm
        s1 = sensoren[1]  # Unterarm
        s2 = sensoren[2]  # Hand

        # Heading normalisieren: 0-360 -> -180..+180
        def norm_heading(h):
            if h > 180:
                return h - 360
            return h

        # Grad zu Radiant
        deg2rad = math.pi / 180.0

        # Joint 1: Base — Oberarm Heading (Yaw)
        base = self.clamp(norm_heading(s0['h']) * deg2rad, -1.5708, 1.5708)

        # Joint 2: Shoulder — Oberarm Pitch
        shoulder = self.clamp(s0['p'] * deg2rad, -0.0873, 1.3963)

        # Joint 3: Elbow — Unterarm Pitch (relativ zu Oberarm)
        elbow = self.clamp(s1['p'] * deg2rad, -0.0873, 1.4835)

        # Joint 4: Wrist — Hand Heading (relativ zu Unterarm)
        wrist = self.clamp(norm_heading(s2['h']) * deg2rad, -1.5708, 1.5708)

        # Joint 5: Gripper — Flex-Sensor als Proxy
        flex = data.get('f', 0.0) / 100.0  # 0-100% -> 0-1
        gripper = self.clamp(flex * 0.6, 0.0, 0.6)

        # JointState publishen
        js = JointState()
        js.header.stamp = self.get_clock().now().to_msg()
        js.name = self.active_joint_names
        js.position = [base, shoulder, elbow, wrist, gripper]

        self.joint_pub.publish(js)

    def clamp(self, val, min_val, max_val):
        return max(min_val, min(max_val, val))

    def status_log(self):
        self.get_logger().info(
            f'Pakete: {self.msg_count} | Zielarm: {self.target_arm}')

    def destroy_node(self):
        self.mqtt_client.loop_stop()
        self.mqtt_client.disconnect()
        super().destroy_node()


def main(args=None):
    rclpy.init(args=args)
    node = MqttBridge()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
