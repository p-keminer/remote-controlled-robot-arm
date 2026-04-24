#!/usr/bin/env python3

import json
import os
from datetime import datetime

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import JointState
from std_msgs.msg import String


class DebugRecorder(Node):
    def __init__(self):
        super().__init__('debug_recorder')

        self.declare_parameter('output_dir', os.path.join(os.getcwd(), 'debug_sessions'))
        self.output_dir = self.get_parameter('output_dir').value

        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        self.session_dir = os.path.join(self.output_dir, timestamp)
        os.makedirs(self.session_dir, exist_ok=True)

        self.raw_file = open(
            os.path.join(self.session_dir, 'raw_imu.jsonl'),
            'a',
            encoding='utf-8',
            buffering=1,
        )
        self.mapping_file = open(
            os.path.join(self.session_dir, 'mapping.jsonl'),
            'a',
            encoding='utf-8',
            buffering=1,
        )
        self.joint_file = open(
            os.path.join(self.session_dir, 'joint_states.jsonl'),
            'a',
            encoding='utf-8',
            buffering=1,
        )

        self.counts = {
            'raw': 0,
            'mapping': 0,
            'joint_states': 0,
        }

        self.create_subscription(String, '/robotarm/debug/raw_imu', self.on_raw_imu, 50)
        self.create_subscription(String, '/robotarm/debug/mapping', self.on_mapping, 50)
        self.create_subscription(JointState, '/joint_states', self.on_joint_state, 50)

        manifest = {
            'created_at': datetime.now().isoformat(timespec='seconds'),
            'session_dir': self.session_dir,
            'topics': [
                '/robotarm/debug/raw_imu',
                '/robotarm/debug/mapping',
                '/joint_states',
            ],
        }
        with open(os.path.join(self.session_dir, 'session.json'), 'w', encoding='utf-8') as handle:
            json.dump(manifest, handle, indent=2)

        self.get_logger().info(f'Debug-Recorder schreibt nach {self.session_dir}')
        self.create_timer(5.0, self.log_status)

    def _parse_json(self, text):
        try:
            return json.loads(text)
        except json.JSONDecodeError:
            return {'raw_text': text}

    def _write_line(self, handle, payload):
        handle.write(json.dumps(payload, separators=(',', ':')) + '\n')

    def on_raw_imu(self, msg):
        self.counts['raw'] += 1
        self._write_line(self.raw_file, {
            'stamp_ns': self.get_clock().now().nanoseconds,
            'payload': self._parse_json(msg.data),
        })

    def on_mapping(self, msg):
        self.counts['mapping'] += 1
        self._write_line(self.mapping_file, {
            'stamp_ns': self.get_clock().now().nanoseconds,
            'payload': self._parse_json(msg.data),
        })

    def on_joint_state(self, msg):
        self.counts['joint_states'] += 1
        self._write_line(self.joint_file, {
            'stamp_ns': self.get_clock().now().nanoseconds,
            'header': {
                'sec': msg.header.stamp.sec,
                'nanosec': msg.header.stamp.nanosec,
            },
            'name': list(msg.name),
            'position': list(msg.position),
        })

    def log_status(self):
        self.get_logger().info(
            'Recorder raw=%d mapping=%d joint_states=%d'
            % (
                self.counts['raw'],
                self.counts['mapping'],
                self.counts['joint_states'],
            )
        )

    def destroy_node(self):
        self.raw_file.close()
        self.mapping_file.close()
        self.joint_file.close()
        super().destroy_node()


def main(args=None):
    rclpy.init(args=args)
    node = DebugRecorder()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
