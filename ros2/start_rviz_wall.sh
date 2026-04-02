#!/bin/bash
# Roboterarm Wandmontage-Modus in RViz2 (mit Slidern, ohne Hardware)
export FASTRTPS_DEFAULT_PROFILES_FILE=~/projects/embedded/robotic-arm/ros2/fastdds_profile.xml
source /opt/ros/jazzy/setup.bash
source ~/projects/embedded/robotic-arm/ros2/install/setup.bash
ros2 launch robotarm_description view_wall.launch.py
