#!/usr/bin/env python3
"""
Launch-File: Wand-URDF in RViz2 mit Live-MQTT-Daten anzeigen.

Startet:
  1. robot_state_publisher (URDF -> TF)
  2. mqtt_bridge (MQTT -> /joint_states)
  3. rviz2 (Visualisierung)
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
        DeclareLaunchArgument('mqtt_host', default_value='localhost'),
        DeclareLaunchArgument('mqtt_port', default_value='1883'),
        DeclareLaunchArgument('mqtt_user', default_value='mcp_reader'),
        DeclareLaunchArgument('mqtt_pass', default_value=''),
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
            executable='mqtt_bridge',
            name='mqtt_bridge',
            parameters=[{
                'mqtt_host': LaunchConfiguration('mqtt_host'),
                'mqtt_port': LaunchConfiguration('mqtt_port'),
                'mqtt_user': LaunchConfiguration('mqtt_user'),
                'mqtt_pass': LaunchConfiguration('mqtt_pass'),
                'target_arm': LaunchConfiguration('target_arm'),
                'wall_mode': True,
            }],
            output='screen',
        ),
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=[
                '-geometry', '1180x820+620+140',
                '-d', os.path.join(pkg_dir, 'config', 'display_wall.rviz'),
            ],
            output='screen',
        ),
    ])
