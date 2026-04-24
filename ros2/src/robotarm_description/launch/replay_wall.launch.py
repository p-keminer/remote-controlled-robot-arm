#!/usr/bin/env python3
"""
Launch-File: Wand-URDF in RViz2 mit Replay aus aufgezeichneten IMU-Daten.
"""

import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    pkg_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    urdf_file = os.path.join(pkg_dir, 'urdf', 'robotarm_wall.urdf')

    with open(urdf_file, 'r', encoding='utf-8') as file_handle:
        robot_description = file_handle.read()

    return LaunchDescription([
        DeclareLaunchArgument('input_file', default_value=''),
        DeclareLaunchArgument('rate', default_value='1.0'),
        DeclareLaunchArgument('loop', default_value='false'),
        DeclareLaunchArgument('target_arm', default_value='both'),
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            parameters=[{'robot_description': robot_description}],
            output='screen',
        ),
        Node(
            package='robotarm_description',
            executable='imu_replayer',
            name='imu_replayer',
            parameters=[{
                'input_file': LaunchConfiguration('input_file'),
                'rate': LaunchConfiguration('rate'),
                'loop': LaunchConfiguration('loop'),
                'target_arm': LaunchConfiguration('target_arm'),
                'wall_mode': True,
                'publish_debug': True,
            }],
            output='screen',
        ),
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', os.path.join(pkg_dir, 'config', 'display_wall.rviz')],
            output='screen',
        ),
    ])
