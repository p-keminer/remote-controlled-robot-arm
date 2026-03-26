# mqtt_config.template.py — Vorlage fuer die MQTT MCP Server Konfiguration
# Kopiere diese Datei als mqtt_config.local.py und passe die Werte an.
# mqtt_config.local.py ist per .gitignore geblockt und darf NICHT eingecheckt werden.

MQTT_BROKER = "BROKER_IP_HIER_EINTRAGEN"
MQTT_PORT = 1883
MQTT_USER = "mcp_reader"
MQTT_PASSWORT = "MEIN_MCP_PASSWORT"
MQTT_TOPICS = ["robotarm/#"]
