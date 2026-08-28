#!/bin/bash
# Wandmontage-Replay aus einer aufgezeichneten Debug-Session starten.
set -euo pipefail

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <raw_imu.jsonl> [rate]"
    exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
INPUT_FILE="$1"
RATE="${2:-1.0}"
LOCAL_CONFIG="$SCRIPT_DIR/robotarm_mqtt.local.sh"

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

ros2 launch robotarm_description replay_wall.launch.py \
    input_file:="$INPUT_FILE" \
    rate:="$RATE" \
    target_arm:="${ROBOTARM_TARGET_ARM:-both}"
