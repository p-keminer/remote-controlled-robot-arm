#!/bin/bash
# Vorlage fuer lokale MQTT-Konfiguration.
# Nach robotarm_mqtt.local.sh kopieren und Werte lokal anpassen.

export ROBOTARM_MQTT_HOST="dietpi"
export ROBOTARM_MQTT_PORT="1883"
export ROBOTARM_MQTT_USER="mcp_reader"
export ROBOTARM_MQTT_PASS="MEIN_MQTT_PASSWORT"
export ROBOTARM_TARGET_ARM="both"
export ROBOTARM_RECORD_DEBUG="true"
export ROBOTARM_SHOW_LIVE_MONITOR="true"
export ROBOTARM_DEBUG_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/debug_sessions"
