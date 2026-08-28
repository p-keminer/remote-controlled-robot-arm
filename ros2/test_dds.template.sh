#!/bin/bash
# DDS-Diagnose: Prueft ob Fast-DDS SHM Transport in WSL2 funktioniert.
# Kopiere zu test_dds.local.sh und passe REPO_PFAD an.
REPO_PFAD=/PFAD/ZUM/REPO/ros2

export FASTRTPS_DEFAULT_PROFILES_FILE="${REPO_PFAD}/fastdds_profile.xml"
source /opt/ros/jazzy/setup.bash
source "${REPO_PFAD}/install/setup.bash"

ros2 run robot_state_publisher robot_state_publisher \
  --ros-args -p robot_description:="$(cat "${REPO_PFAD}/install/robotarm_description/share/robotarm_description/urdf/robotarm.urdf")" &

sleep 5
echo "=== TOPICS ==="
ros2 topic list --no-daemon
echo "=== DONE ==="
kill %1 2>/dev/null
