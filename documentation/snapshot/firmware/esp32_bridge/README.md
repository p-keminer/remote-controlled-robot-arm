# firmware/esp32_bridge

## Zweck

Dieser Ordner enthaelt die Firmware fuer den Bridge-ESP32, der als drahtloser Debug-Beobachter fungiert.
Der Bridge-ESP32 empfaengt ImuPaket v3 per ESP-NOW vom Controller und leitet die Daten per WiFi und MQTT an den Mosquitto-Broker auf dem Raspberry Pi weiter.

Die Bridge ist ein reines **Entwicklungswerkzeug** und kein Teil des v1-Steuerpfads.
Sie kann keine Befehle zuruecksenden und hat keinen Einfluss auf die Servobewegung.

## Aktueller Stand

Die Bridge-Firmware ist implementiert und bench-validiert.
ESP-NOW Empfang, MQTT-Weiterleitung (PubSubClient), OTA (ArduinoOTA) und RGB-FAULT-LED (NeoPixel) funktionieren.
Alle ESPs laufen auf WiFi-Kanal 1 (Router-Kanal) fuer ESP-NOW/WiFi-Koexistenz.
LED-Schema invertiert: aus = OK, blinken = Problem.

## Inhalt

- `esp32_bridge.ino` — Hauptfirmware: ESP-NOW Empfang, MQTT Publish, OTA, RGB-FAULT
- `peer_config.template.h` — Vorlage fuer die Controller-MAC-Adresse (Absendervalidierung)
- `wifi_config.template.h` — Vorlage fuer WiFi SSID, Passwort, MQTT-Broker IP/Port/User/Passwort, OTA-Passwort

## Board

FQBN: `esp32:esp32:robotic_arm_s3n16r8` — **nie** das generische `esp32s3` verwenden (siehe `../../GLOBAL_RULES.md`).

## Regeln

- die Bridge ist rein empfangend und darf keine Daten in den Steuerpfad einschleusen
- WiFi- und MQTT-Zugangsdaten gehoeren ausschliesslich in die gitignorierte `wifi_config.local.h`
- die MAC-Adresse des Controllers gehoert in die gitignorierte `peer_config.local.h`
- OTA-Passwort muss sich von WiFi- und MQTT-Passwort unterscheiden (drei getrennte Geheimnisse)
- ESP-NOW und WiFi muessen auf dem gleichen Funkkanal laufen (aktuell Kanal 1)
- ein Ausfall der Bridge darf den Controller-Receiver-Pfad nicht beeintraechtigen
- die Bridge validiert empfangene Pakete (Groesse, Absender-MAC, Pruefsumme, Protokollversion) bevor sie per MQTT weiterleitet

## LED-Debugging (invertiert: aus = OK, blinken = Problem)

- GPIO4 Gruen — blinkt wenn WiFi getrennt
- GPIO5 Blau — blinkt wenn ESP-NOW Timeout (kein Paket seit 2s)
- GPIO7 Weiss — blinkt wenn MQTT getrennt
- GPIO48 RGB — rot blinkend bei FAULT (irgendein Problem)

## MQTT Topics

- `robotarm/imu` — ImuPaket v3 als kompaktes JSON (bei jedem Empfang, QoS 0)
- `robotarm/status` — Bridge-Status: WiFi RSSI, Uptime, Paketrate, Fehlerrate (1Hz, retained)
- `robotarm/kalib` — Kalibrierungsstatus pro Sensor (bei Aenderung, retained)
- `robotarm/ota/log` — OTA-Versuchsprotokoll (bei jedem Versuch)

## Schnittstellen/Abhaengigkeiten

- empfaengt ImuPaket v3 per ESP-NOW vom Controller (`../esp32_controller/`)
- publiziert per MQTT auf dem Mosquitto-Broker des Pi (`../../dashboard/`)
- nutzt die gleiche Paketstruktur wie `../esp32_controller/` und `../esp32_receiver/`
- WiFi-Kanal 1 muss zum Router-Kanal und zum Controller/Receiver passen
- Paketformat dokumentiert in `../../COMMUNICATION_FRAMEWORK.md`
- MQTT-Topic-Struktur dokumentiert in `../../dashboard/DASHBOARD_CONCEPT.md`
