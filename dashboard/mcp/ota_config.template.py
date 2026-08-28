# ota_config.template.py — Vorlage fuer die OTA-Konfiguration im MCP Server
# Kopiere diese Datei als ota_config.local.py und passe die Werte an.
# ota_config.local.py ist per .gitignore geblockt und darf NICHT eingecheckt werden.
#
# ACHTUNG: OTA ist ein sicherheitskritischer Angriffsvektor!
# Siehe DASHBOARD_CONCEPT.md Abschnitt "OTA-Sicherheitskonzept" fuer Details.

# Bridge-ESP32 Netzwerkadresse
OTA_BRIDGE_IP = "BRIDGE_IP_HIER_EINTRAGEN"
OTA_BRIDGE_PORT = 3232  # ArduinoOTA Default-Port

# OTA-Passwort (MUSS identisch mit OTA_PASSWORT in wifi_config.local.h sein)
# MUSS sich von WiFi- und MQTT-Passwort unterscheiden!
OTA_PASSWORT = "MEIN_OTA_PASSWORT"

# Lokaler Pfad zum Firmware-Verzeichnis (fuer Kompilierung)
# Wird relativ zum Projektroot aufgeloest
FIRMWARE_PFAD = "firmware/esp32_bridge"
BOARD_FQBN = "esp32:esp32:robotic_arm_s3n16r8"
