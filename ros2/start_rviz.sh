#!/bin/bash
# Roboterarm stehend in RViz2 anzeigen (mit Slidern, ohne Hardware)
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
export FASTRTPS_DEFAULT_PROFILES_FILE="$SCRIPT_DIR/fastdds_profile.xml"
source /opt/ros/jazzy/setup.bash
source "$SCRIPT_DIR/install/setup.bash"
ros2 launch robotarm_description view.launch.py
