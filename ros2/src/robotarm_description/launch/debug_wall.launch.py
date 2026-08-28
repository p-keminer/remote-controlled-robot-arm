#!/usr/bin/env python3
"""
Launch-File: Wand-URDF in RViz2 mit Live-MQTT-Daten und Debug-Aufzeichnung.
"""

import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
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
        DeclareLaunchArgument('record_debug', default_value='true'),
        DeclareLaunchArgument('show_live_monitor', default_value='true'),
        DeclareLaunchArgument('debug_dir', default_value=os.path.join(pkg_dir, '..', '..', '..', 'debug_sessions')),
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
                'publish_debug': True,
            }],
            output='screen',
        ),
        Node(
            package='robotarm_description',
            executable='debug_recorder',
            name='debug_recorder',
            parameters=[{
                'output_dir': LaunchConfiguration('debug_dir'),
            }],
            condition=IfCondition(LaunchConfiguration('record_debug')),
            output='screen',
        ),
        Node(
            package='robotarm_description',
            executable='live_monitor',
            name='live_monitor',
            parameters=[{
                'window_seconds': 30.0,
                'max_points': 1800,
                'window_x': 40,
                'window_y': 140,
            }],
            condition=IfCondition(LaunchConfiguration('show_live_monitor')),
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
