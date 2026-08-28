#!/bin/bash
# Raeumt alte ROS/RViz/Monitor-Prozesse fuer den Wand-Stack weg.
set -euo pipefail

kill_matching() {
    local pattern="$1"
    local pids
    pids="$(pgrep -f "$pattern" || true)"
    if [[ -n "$pids" ]]; then
        kill -9 $pids >/dev/null 2>&1 || true
    fi
}

kill_matching "ros2 launch robotarm_description debug_wall.launch.py"
kill_matching "ros2 launch robotarm_description display_wall.launch.py"
kill_matching "robot_state_publisher"
kill_matching "mqtt_bridge"
kill_matching "debug_recorder"
kill_matching "live_monitor"
kill_matching "rviz2"

sleep 2
