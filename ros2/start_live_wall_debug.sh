#!/bin/bash
# Wandmontage live mit Debug-Aufzeichnung starten.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
LOCAL_CONFIG="$SCRIPT_DIR/robotarm_mqtt.local.sh"

bash "$SCRIPT_DIR/stop_live_wall_stack.sh"

if [[ -f "$LOCAL_CONFIG" ]]; then
    set -a
    source "$LOCAL_CONFIG"
    set +a
fi

export FASTRTPS_DEFAULT_PROFILES_FILE="$SCRIPT_DIR/fastdds_profile.xml"
set +u
source /opt/ros/jazzy/setup.bash
source "$SCRIPT_DIR/install/setup.bash"
set -u

ros2 launch robotarm_description debug_wall.launch.py \
    mqtt_host:="${ROBOTARM_MQTT_HOST:-localhost}" \
    mqtt_port:="${ROBOTARM_MQTT_PORT:-1883}" \
    mqtt_user:="${ROBOTARM_MQTT_USER:-mcp_reader}" \
    mqtt_pass:="${ROBOTARM_MQTT_PASS:-}" \
    target_arm:="${ROBOTARM_TARGET_ARM:-both}" \
    debug_dir:="${ROBOTARM_DEBUG_DIR:-$SCRIPT_DIR/debug_sessions}" \
    record_debug:="${ROBOTARM_RECORD_DEBUG:-true}" \
    show_live_monitor:="${ROBOTARM_SHOW_LIVE_MONITOR:-true}"
