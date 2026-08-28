#!/bin/bash
# Erzeuge ein Kurvendiagramm aus der neuesten oder angegebenen Debug-Session.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SESSION_ARG="${1:-}"
SESSIONS_DIR="$SCRIPT_DIR/debug_sessions"

export FASTRTPS_DEFAULT_PROFILES_FILE="$SCRIPT_DIR/fastdds_profile.xml"
set +u
source /opt/ros/jazzy/setup.bash
source "$SCRIPT_DIR/install/setup.bash"
set -u

if [[ -n "$SESSION_ARG" ]]; then
    ros2 run robotarm_description debug_plot "$SESSION_ARG"
else
    ros2 run robotarm_description debug_plot --latest-under "$SESSIONS_DIR"
fi
