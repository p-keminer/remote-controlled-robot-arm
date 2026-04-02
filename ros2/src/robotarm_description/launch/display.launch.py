#!/usr/bin/env python3
"""
Launch-File: URDF in RViz2 mit Live-MQTT-Daten anzeigen.

Startet:
  1. robot_state_publisher (URDF → TF)
  2. mqtt_bridge (MQTT → /joint_states)
  3. rviz2 (Visualisierung)
"""

import os
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration


def generate_launch_description():
    pkg_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    urdf_file = os.path.join(pkg_dir, 'urdf', 'robotarm.urdf')

    with open(urdf_file, 'r') as f:
        robot_description = f.read()

    return LaunchDescription([
        # MQTT Parameter
        DeclareLaunchArgument('mqtt_host', default_value='localhost'),
        DeclareLaunchArgument('mqtt_port', default_value='1883'),
        DeclareLaunchArgument('mqtt_user', default_value='dashboard_ws'),
        DeclareLaunchArgument('mqtt_pass', default_value=''),

        # Robot State Publisher (URDF → TF-Baum)
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            parameters=[{'robot_description': robot_description}],
            output='screen',
        ),

        # MQTT Bridge (MQTT → /joint_states)
        Node(
            package='robotarm_description',
            executable='mqtt_bridge',
            name='mqtt_bridge',
            parameters=[{
                'mqtt_host': LaunchConfiguration('mqtt_host'),
                'mqtt_port': 1883,
                'mqtt_user': LaunchConfiguration('mqtt_user'),
                'mqtt_pass': LaunchConfiguration('mqtt_pass'),
            }],
            output='screen',
        ),

        # RViz2
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', os.path.join(pkg_dir, 'config', 'display.rviz')],
            output='screen',
        ),
    ])
