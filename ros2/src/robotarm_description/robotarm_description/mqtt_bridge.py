#!/usr/bin/env python3
"""MQTT-zu-ROS2 Bridge fuer den Adeept 5-DOF Roboterarm."""

import json

import paho.mqtt.client as mqtt
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import JointState
from std_msgs.msg import String
from std_srvs.srv import Trigger

from robotarm_description.joint_state_utils import build_joint_state, joint_name_sets
from robotarm_description.mapping import map_imu_payload, stabilize_gripper_angle


class MqttBridge(Node):
    def __init__(self):
        super().__init__('mqtt_bridge')

        self.declare_parameter('mqtt_host', 'localhost')
        self.declare_parameter('mqtt_port', 1883)
        self.declare_parameter('mqtt_user', 'mcp_reader')
        self.declare_parameter('mqtt_pass', '')
        self.declare_parameter('mqtt_topic', 'robotarm/imu')
        self.declare_parameter('target_arm', 'front')
        self.declare_parameter('wall_mode', False)
        self.declare_parameter('publish_debug', False)

        self.mqtt_host = self.get_parameter('mqtt_host').value
        self.mqtt_port = self.get_parameter('mqtt_port').value
        self.mqtt_user = self.get_parameter('mqtt_user').value
        self.mqtt_pass = self.get_parameter('mqtt_pass').value
        self.mqtt_topic = self.get_parameter('mqtt_topic').value
        self.target_arm = self.get_parameter('target_arm').value
        self.wall_mode = self.get_parameter('wall_mode').value
        self.publish_debug = self.get_parameter('publish_debug').value
        self.filtered_gripper_position = None

        self.joint_pub = self.create_publisher(JointState, '/joint_states', 10)
        self.raw_pub = None
        self.mapping_pub = None
        if self.publish_debug:
            self.raw_pub = self.create_publisher(String, '/robotarm/debug/raw_imu', 50)
            self.mapping_pub = self.create_publisher(String, '/robotarm/debug/mapping', 50)

        self.joint_names_front, self.joint_names_back = joint_name_sets(self.wall_mode)

        self.toggle_srv = self.create_service(
            Trigger,
            'toggle_arm',
            self.toggle_arm_callback,
        )

        self.mqtt_client = mqtt.Client(
            client_id='ros2_bridge',
            callback_api_version=mqtt.CallbackAPIVersion.VERSION2,
        )
        if self.mqtt_user:
            self.mqtt_client.username_pw_set(self.mqtt_user, self.mqtt_pass)
        self.mqtt_client.on_connect = self.on_mqtt_connect
        self.mqtt_client.on_message = self.on_mqtt_message
        self.mqtt_client.on_disconnect = self.on_mqtt_disconnect

        self.get_logger().info(
            f'Verbinde mit MQTT {self.mqtt_host}:{self.mqtt_port}...'
        )
        self.get_logger().info(
            'Zielarm: '
            f'{self.target_arm} '
            '(toggle via: ros2 service call /toggle_arm std_srvs/srv/Trigger)'
        )

        try:
            self.mqtt_client.connect_async(self.mqtt_host, self.mqtt_port)
            self.mqtt_client.loop_start()
        except Exception as exc:
            self.get_logger().error(f'MQTT-Verbindungsfehler: {exc}')

        self.msg_count = 0
        self.create_timer(5.0, self.status_log)

    def toggle_arm_callback(self, request, response):
        del request
        if self.target_arm == 'front':
            self.target_arm = 'back'
        elif self.target_arm == 'back':
            self.target_arm = 'both'
        else:
            self.target_arm = 'front'
        response.success = True
        response.message = f'Zielarm: {self.target_arm}'
        self.get_logger().info(f'Arm gewechselt -> {self.target_arm}')
        return response

    def on_mqtt_connect(self, client, userdata, flags, rc, properties=None):
        del userdata, flags, properties
        if rc == 0:
            self.get_logger().info('MQTT verbunden!')
            client.subscribe(self.mqtt_topic)
        else:
            self.get_logger().error(f'MQTT-Verbindung fehlgeschlagen: rc={rc}')

    def on_mqtt_disconnect(self, client, userdata, flags, rc, properties=None):
        del client, userdata, flags, rc, properties
        self.get_logger().warn('MQTT getrennt - Reconnect...')

    def on_mqtt_message(self, client, userdata, msg):
        del client, userdata
        try:
            payload_text = msg.payload.decode()
            data = json.loads(payload_text)
            self.publish_joint_state(data, payload_text=payload_text)
            self.msg_count += 1
        except (json.JSONDecodeError, KeyError, TypeError, ValueError) as exc:
            self.get_logger().warn(f'Paket-Fehler: {exc}')

    def publish_joint_state(self, data, payload_text=None):
        if self.publish_debug:
            joint_positions, debug_payload = map_imu_payload(data, include_debug=True)
        else:
            joint_positions = map_imu_payload(data, include_debug=False)
            debug_payload = None

        joint_positions[4] = stabilize_gripper_angle(
            joint_positions[4],
            self.filtered_gripper_position,
        )
        self.filtered_gripper_position = joint_positions[4]

        joint_state = build_joint_state(
            self.get_clock().now().to_msg(),
            joint_positions,
            wall_mode=self.wall_mode,
            target_arm=self.target_arm,
        )
        self.joint_pub.publish(joint_state)

        if self.publish_debug and self.raw_pub is not None and self.mapping_pub is not None:
            raw_msg = String()
            raw_msg.data = payload_text if payload_text is not None else json.dumps(data, separators=(',', ':'))
            self.raw_pub.publish(raw_msg)

            debug_payload['gripper']['stabilized_angle'] = joint_positions[4]
            debug_payload['joint_positions']['gripper'] = joint_positions[4]
            debug_payload['source'] = 'mqtt'
            debug_payload['target_arm'] = self.target_arm
            debug_payload['wall_mode'] = bool(self.wall_mode)

            mapping_msg = String()
            mapping_msg.data = json.dumps(debug_payload, separators=(',', ':'))
            self.mapping_pub.publish(mapping_msg)

    def status_log(self):
        self.get_logger().info(
            f'Pakete: {self.msg_count} | Zielarm: {self.target_arm}'
        )

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
