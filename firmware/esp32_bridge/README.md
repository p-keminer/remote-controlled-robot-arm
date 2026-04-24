# firmware/esp32_bridge

## Zweck

Dieser Ordner enthaelt die Firmware fuer den Bridge-ESP32.
Die Bridge ist rein lesend und transportiert Debugdaten aus dem ESP-NOW-Pfad zu MQTT auf dem Pi.

## Aktueller Stand

Die Bridge ist der aktive Debugpfad des Projekts und wurde end-to-end verifiziert:

- ESP-NOW-Empfang vom Controller
- MQTT-Publish nach Mosquitto auf dem Pi
- Nutzung durch Dashboard, ROS und MQTT-Debugwerkzeuge
- WiFi/MQTT-Statusanzeigen ueber LEDs

Die Bridge bleibt ausserhalb des eigentlichen Steuerpfads.
Ein Ausfall der Bridge darf keine direkte Servoausfuehrung verhindern.

## Inhalt

- `esp32_bridge.ino` fuer ESP-NOW, MQTT und Statuslogik
- `peer_config.template.h` fuer die Controller-MAC als Vorlage
- `wifi_config.template.h` fuer lokale WiFi-/MQTT-/OTA-Konfiguration

## Regeln

- Die Bridge sendet nichts in den Steuerpfad zurueck.
- Zugangsdaten und MACs liegen nur in gitignorierten lokalen Dateien.
- WiFi/MQTT bleibt ein Debug- und Beobachtungspfad, kein Safety-Pfad.

## Schnittstellen/Abhaengigkeiten

- liest von `../esp32_controller/`
- publiziert fuer `../../dashboard/` und `../../ros2/`
- folgt `../../COMMUNICATION_FRAMEWORK.md` und `../../security/`
