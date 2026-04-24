#!/usr/bin/env python3

import json
import os
import time

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import JointState
from std_msgs.msg import String

from robotarm_description.joint_state_utils import build_joint_state
from robotarm_description.mapping import map_imu_payload


def as_bool(value):
    if isinstance(value, bool):
        return value
    return str(value).strip().lower() in ('1', 'true', 'yes', 'on')


class ImuReplayer(Node):
    def __init__(self):
        super().__init__('imu_replayer')

        self.declare_parameter('input_file', '')
        self.declare_parameter('rate', 1.0)
        self.declare_parameter('loop', False)
        self.declare_parameter('target_arm', 'front')
        self.declare_parameter('wall_mode', True)
        self.declare_parameter('publish_debug', True)

        self.input_file = self.get_parameter('input_file').value
        self.rate = float(self.get_parameter('rate').value)
        self.loop = as_bool(self.get_parameter('loop').value)
        self.target_arm = self.get_parameter('target_arm').value
        self.wall_mode = as_bool(self.get_parameter('wall_mode').value)
        self.publish_debug = as_bool(self.get_parameter('publish_debug').value)

        if not self.input_file:
            raise ValueError('Parameter input_file is required')
        if not os.path.exists(self.input_file):
            raise FileNotFoundError(self.input_file)
        if self.rate <= 0.0:
            raise ValueError('Parameter rate must be > 0')

        self.records = self._load_records(self.input_file)
        if not self.records:
            raise ValueError('Replay file contains no records')

        self.joint_pub = self.create_publisher(JointState, '/joint_states', 10)
        self.raw_pub = None
        self.mapping_pub = None
        if self.publish_debug:
            self.raw_pub = self.create_publisher(String, '/robotarm/debug/raw_imu', 50)
            self.mapping_pub = self.create_publisher(String, '/robotarm/debug/mapping', 50)

        self.record_index = 0
        self.next_emit_time = time.monotonic()
        self.timer = self.create_timer(0.01, self.tick)

        self.get_logger().info(
            f'Replay geladen: {len(self.records)} Pakete aus {self.input_file} '
            f'@ {self.rate:.2f}x'
        )

    def _load_records(self, path):
        loaded = []
        with open(path, 'r', encoding='utf-8') as handle:
            for line in handle:
                line = line.strip()
                if not line:
                    continue
                record = json.loads(line)
                payload = record.get('payload')
                if not isinstance(payload, dict):
                    continue
                loaded.append({
                    'stamp_ns': int(record.get('stamp_ns', 0)),
                    'payload': payload,
                })
        return loaded

    def _publish_one(self, record):
        if self.publish_debug and self.raw_pub is not None:
            raw_msg = String()
            raw_msg.data = json.dumps(record['payload'], separators=(',', ':'))
            self.raw_pub.publish(raw_msg)

        joint_positions, debug_payload = map_imu_payload(record['payload'], include_debug=self.publish_debug)
        joint_state = build_joint_state(
            self.get_clock().now().to_msg(),
            joint_positions,
            wall_mode=self.wall_mode,
            target_arm=self.target_arm,
        )
        self.joint_pub.publish(joint_state)

        if self.publish_debug and self.mapping_pub is not None:
            debug_payload['source'] = 'replay'
            debug_payload['target_arm'] = self.target_arm
            debug_payload['wall_mode'] = bool(self.wall_mode)
            debug_msg = String()
            debug_msg.data = json.dumps(debug_payload, separators=(',', ':'))
            self.mapping_pub.publish(debug_msg)

    def tick(self):
        now = time.monotonic()
        if now < self.next_emit_time:
            return

        record = self.records[self.record_index]
        self._publish_one(record)

        if self.record_index + 1 >= len(self.records):
            if not self.loop:
                self.get_logger().info('Replay abgeschlossen')
                self.timer.cancel()
                return
            self.record_index = 0
            self.next_emit_time = now
            self.get_logger().info('Replay neu gestartet')
            return

        current_stamp = record['stamp_ns']
        self.record_index += 1
        next_stamp = self.records[self.record_index]['stamp_ns']
        delta_ns = max(1, next_stamp - current_stamp)
        self.next_emit_time = now + (delta_ns / 1_000_000_000.0) / self.rate


def main(args=None):
    rclpy.init(args=args)
    node = ImuReplayer()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
