#!/usr/bin/env python3

import json
import math
import threading
import time
from collections import deque

import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

import rclpy
from rclpy.executors import SingleThreadedExecutor
from rclpy.node import Node
from sensor_msgs.msg import JointState
from std_msgs.msg import String


RAW_SERIES = {
    'S2 pitch': ('raw_s2_pitch', '#1f77b4'),
    'S2 heading': ('raw_s2_heading', '#ff7f0e'),
    'S1 heading': ('raw_s1_heading', '#2ca02c'),
    'Flex': ('raw_flex', '#d62728'),
}

MAPPED_SERIES = {
    'Base yaw': ('map_base_yaw', '#1f77b4'),
    'Shoulder': ('map_shoulder', '#ff7f0e'),
    'Elbow': ('map_elbow', '#2ca02c'),
    'Wrist': ('map_wrist', '#d62728'),
}

INTERMEDIATE_SERIES = {
    'Base input': ('mid_base_input', '#1f77b4'),
    'Shoulder input': ('mid_shoulder_input', '#ff7f0e'),
    'Elbow input': ('mid_elbow_input', '#2ca02c'),
    'Wrist twist': ('mid_wrist_twist', '#d62728'),
}

JOINT_SERIES = {
    'Base joint': ('joint_base', '#1f77b4'),
    'Shoulder joint': ('joint_shoulder', '#ff7f0e'),
    'Elbow joint': ('joint_elbow', '#2ca02c'),
    'Wrist joint': ('joint_wrist', '#d62728'),
    'Gripper': ('joint_gripper', '#9467bd'),
}


class LiveMonitor(Node):
    def __init__(self):
        super().__init__('live_monitor')

        self.declare_parameter('window_seconds', 30.0)
        self.declare_parameter('max_points', 1800)
        self.declare_parameter('window_x', 40)
        self.declare_parameter('window_y', 140)
        self.window_seconds = float(self.get_parameter('window_seconds').value)
        self.max_points = int(self.get_parameter('max_points').value)
        self.window_x = int(self.get_parameter('window_x').value)
        self.window_y = int(self.get_parameter('window_y').value)

        self._lock = threading.Lock()
        self._start_time = None
        self.series = {
            key: deque(maxlen=self.max_points)
            for key in {
                *(value[0] for value in RAW_SERIES.values()),
                *(value[0] for value in MAPPED_SERIES.values()),
                *(value[0] for value in INTERMEDIATE_SERIES.values()),
                *(value[0] for value in JOINT_SERIES.values()),
            }
        }
        self.latest = {}

        self.create_subscription(String, '/robotarm/debug/raw_imu', self.on_raw, 50)
        self.create_subscription(String, '/robotarm/debug/mapping', self.on_mapping, 50)
        self.create_subscription(JointState, '/joint_states', self.on_joint_state, 50)

    def _now(self):
        now = time.monotonic()
        if self._start_time is None:
            self._start_time = now
        return now - self._start_time

    def _append(self, key, value):
        if value is None:
            return
        if isinstance(value, bool):
            value = float(value)
        elif not isinstance(value, (int, float)):
            return
        self.series[key].append((self._now(), float(value)))
        self.latest[key] = float(value)

    def on_raw(self, msg):
        try:
            payload = json.loads(msg.data)
        except json.JSONDecodeError:
            return
        sensors = payload.get('s', [])
        if len(sensors) < 3:
            return
        with self._lock:
            self._append('raw_s2_pitch', sensors[2].get('p'))
            self._append('raw_s2_heading', sensors[2].get('h'))
            self._append('raw_s1_heading', sensors[1].get('h'))
            self._append('raw_flex', payload.get('f'))

    def on_mapping(self, msg):
        try:
            payload = json.loads(msg.data)
        except json.JSONDecodeError:
            return
        with self._lock:
            self._append('map_base_yaw', self._dig(payload, 'base', 'yaw_deg'))
            self._append('map_shoulder', self._dig(payload, 'shoulder', 'pitch_deg'))
            self._append('map_elbow', self._dig(payload, 'elbow', 'angle_deg'))
            self._append('map_wrist', self._dig(payload, 'wrist', 'angle_deg'))

            self._append('mid_base_input', self._dig(payload, 'base', 'pitch_input'))
            self._append('mid_shoulder_input', self._dig(payload, 'shoulder', 'input'))
            self._append('mid_elbow_input', self._dig(payload, 'elbow', 'input'))
            self._append('mid_wrist_twist', self._dig(payload, 'wrist', 'twist_rel_deg'))

            source = payload.get('source')
            if source:
                self.latest['source'] = source
            target_arm = payload.get('target_arm')
            if target_arm:
                self.latest['target_arm'] = target_arm

    def on_joint_state(self, msg):
        joint_map = dict(zip(msg.name, msg.position))
        base_value = self._joint_value(joint_map, 'base_joint_l', 'base_joint', 'base_joint_r')
        shoulder_value = self._joint_value(joint_map, 'shoulder_joint_l', 'shoulder_joint', 'shoulder_joint_r')
        elbow_value = self._joint_value(joint_map, 'elbow_joint_l', 'elbow_joint', 'elbow_joint_r')
        wrist_value = self._joint_value(joint_map, 'wrist_joint_l', 'wrist_joint', 'wrist_joint_r')
        gripper_value = self._joint_value(joint_map, 'gripper_joint_l', 'gripper_left_joint', 'gripper_joint_r')

        with self._lock:
            self._append('joint_base', math.degrees(base_value) if base_value is not None else None)
            self._append('joint_shoulder', math.degrees(shoulder_value) if shoulder_value is not None else None)
            self._append('joint_elbow', math.degrees(elbow_value) if elbow_value is not None else None)
            self._append('joint_wrist', math.degrees(wrist_value) if wrist_value is not None else None)
            self._append('joint_gripper', gripper_value)

    @staticmethod
    def _joint_value(joint_map, *names):
        for name in names:
            if name in joint_map:
                return joint_map[name]
        return None

    @staticmethod
    def _dig(payload, *keys):
        current = payload
        for key in keys:
            if not isinstance(current, dict) or key not in current:
                return None
            current = current[key]
        return current

    def snapshot(self):
        with self._lock:
            return {
                'series': {key: list(values) for key, values in self.series.items()},
                'latest': dict(self.latest),
            }


def create_monitor_window(node):
    fig, axes = plt.subplots(4, 1, figsize=(11.5, 7.2), sharex=False)
    fig.subplots_adjust(right=0.74, hspace=0.34)
    text_box = fig.text(0.76, 0.98, '', va='top', ha='left', family='monospace', fontsize=9)

    manager = plt.get_current_fig_manager()
    try:
        manager.window.wm_geometry(f"+{node.window_x}+{node.window_y}")
    except Exception:
        pass

    plot_groups = [
        ('Rohdaten', 'Grad / Flex', RAW_SERIES),
        ('Gemappte Gelenkwinkel', 'Grad', MAPPED_SERIES),
        ('Zwischenwerte', 'Grad', INTERMEDIATE_SERIES),
        ('Publizierte JointStates', 'Grad / m', JOINT_SERIES),
    ]

    line_handles = []
    for axis, (title, ylabel, series_defs) in zip(axes, plot_groups):
        axis.set_title(title)
        axis.set_ylabel(ylabel)
        axis.grid(True, alpha=0.3)
        handles = []
        for label, (_, color) in series_defs.items():
            line, = axis.plot([], [], label=label, color=color)
            handles.append((line, series_defs[label][0]))
        axis.legend(loc='upper right')
        line_handles.append(handles)
    axes[-1].set_xlabel('Zeit [s]')

    def update(_frame):
        snap = node.snapshot()
        latest_time = 0.0
        for axis, handles in zip(axes, line_handles):
            local_min = None
            local_max = None
            for line, key in handles:
                points = snap['series'].get(key, [])
                xs = [point[0] for point in points]
                ys = [point[1] for point in points]
                line.set_data(xs, ys)
                if xs:
                    latest_time = max(latest_time, xs[-1])
                if ys:
                    series_min = min(ys)
                    series_max = max(ys)
                    local_min = series_min if local_min is None else min(local_min, series_min)
                    local_max = series_max if local_max is None else max(local_max, series_max)
            x_min = max(0.0, latest_time - node.window_seconds)
            axis.set_xlim(x_min, max(node.window_seconds, latest_time + 0.5))
            if local_min is None or local_max is None:
                axis.set_ylim(-1.0, 1.0)
            else:
                span = max(1.0, local_max - local_min)
                axis.set_ylim(local_min - span * 0.15, local_max + span * 0.15)

        latest = snap['latest']
        text_box.set_text(
            '\n'.join([
                'Live-Werte',
                '',
                f"Quelle     : {latest.get('source', '-')}",
                f"Arm-Modus  : {latest.get('target_arm', '-')}",
                '',
                f"S2 pitch   : {latest.get('raw_s2_pitch', float('nan')):7.2f}",
                f"S2 heading : {latest.get('raw_s2_heading', float('nan')):7.2f}",
                f"S1 heading : {latest.get('raw_s1_heading', float('nan')):7.2f}",
                f"Flex       : {latest.get('raw_flex', float('nan')):7.2f}",
                '',
                f"Base yaw   : {latest.get('map_base_yaw', float('nan')):7.2f}",
                f"Shoulder   : {latest.get('map_shoulder', float('nan')):7.2f}",
                f"Elbow      : {latest.get('map_elbow', float('nan')):7.2f}",
                f"Wrist      : {latest.get('map_wrist', float('nan')):7.2f}",
                '',
                f"Base input : {latest.get('mid_base_input', float('nan')):7.2f}",
                f"Sh. input  : {latest.get('mid_shoulder_input', float('nan')):7.2f}",
                f"El. input  : {latest.get('mid_elbow_input', float('nan')):7.2f}",
                f"Wr. twist  : {latest.get('mid_wrist_twist', float('nan')):7.2f}",
                '',
                f"Base joint : {latest.get('joint_base', float('nan')):7.2f}",
                f"Sh. joint  : {latest.get('joint_shoulder', float('nan')):7.2f}",
                f"El. joint  : {latest.get('joint_elbow', float('nan')):7.2f}",
                f"Wr. joint  : {latest.get('joint_wrist', float('nan')):7.2f}",
                f"Gripper    : {latest.get('joint_gripper', float('nan')):7.3f}",
            ])
        )
        fig.suptitle('Robotarm Live Monitor', fontsize=16)
        return [line for handles in line_handles for line, _ in handles]

    animation = FuncAnimation(fig, update, interval=150, cache_frame_data=False)
    fig._robotarm_animation = animation
    return fig


def main(args=None):
    rclpy.init(args=args)
    node = LiveMonitor()
    executor = SingleThreadedExecutor()
    executor.add_node(node)

    spin_thread = threading.Thread(target=executor.spin, daemon=True)
    spin_thread.start()

    try:
        create_monitor_window(node)
        plt.show()
    finally:
        executor.shutdown()
        node.destroy_node()
        rclpy.shutdown()
        spin_thread.join(timeout=1.0)


if __name__ == '__main__':
    main()
