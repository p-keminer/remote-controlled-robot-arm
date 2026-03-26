// wifi_config.template.h — Vorlage fuer die Bridge-ESP32 WiFi-, MQTT- und OTA-Konfiguration
// Kopiere diese Datei als wifi_config.local.h und ersetze die Platzhalter.
// wifi_config.local.h ist per .gitignore geblockt und darf NICHT eingecheckt werden.

// ============================================================
// WiFi-Zugangsdaten
// ============================================================
#define WIFI_SSID      "MEIN_NETZWERK"
#define WIFI_PASSWORT  "MEIN_PASSWORT"

// ============================================================
// MQTT-Broker (Mosquitto auf dem Pi)
// ============================================================
#define MQTT_BROKER    "BROKER_IP_HIER_EINTRAGEN"
#define MQTT_PORT      1883
#define MQTT_USER      "bridge"
#define MQTT_PASSWORT  "MEIN_MQTT_PASSWORT"

// MQTT Topics
#define TOPIC_IMU      "robotarm/imu"
#define TOPIC_STATUS   "robotarm/status"
#define TOPIC_KALIB    "robotarm/kalib"
#define TOPIC_OTA_LOG  "robotarm/ota/log"

// ============================================================
// OTA-Konfiguration (Over-The-Air Firmware-Update)
// ============================================================
// ACHTUNG: OTA-Passwort MUSS sich von WiFi- und MQTT-Passwort unterscheiden!
// Drei getrennte Geheimnisse fuer drei getrennte Angriffsvektoren.
#define OTA_PASSWORT   "MEIN_OTA_PASSWORT"
#define OTA_HOSTNAME   "robotarm-bridge"

// OTA Rollback: Sekunden bis WiFi/MQTT stehen muessen, sonst Rollback
#define OTA_WIFI_TIMEOUT_S   30
#define OTA_MQTT_TIMEOUT_S   60
