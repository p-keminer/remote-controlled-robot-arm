# firmware

## Zweck

Dieser Ordner enthaelt die gesamte Laufzeitlogik fuer Controller, Receiver, Servoausfuehrung und die Debug-Bridge.

## Aktueller Stand

Controller, Receiver und Bridge sind als Bench-Firmware vorhanden und bench-validiert.
Controller sendet ImuPaket v4 (59 Bytes, mit flags-Bitfeld) per ESP-NOW an Receiver (Steuerpfad) und Bridge (Debug-Pfad) gleichzeitig.
Notaus-Toggle-Button am Controller (GPIO21) propagiert das Notaus-Flag per ImuPaket an alle Peers.
LED-Schema ist invertiert: aus = OK, an = Problem. RGB auf GPIO48: orange blinkend bei Notaus, rot blinkend bei Fehler, aus wenn OK.
Arduino-Servoebene, Security-Uplift und finale Trennung der Laufzeitpfade sind noch offen.
Firmware-Archive frueherer Versionen liegen unter `espnow_imu_v1/`, `espnow_imu_v2/`, `espnow_receiver_v1/`, `espnow_receiver_v2/`.

## Inhalt

- `esp32_controller/` fuer Wearable-Sensorik, Vorverarbeitung und Funksendung (Multi-Peer: Receiver + Bridge)
- `esp32_receiver/` fuer Funkempfang, Validierung und UART-Uebergabe (Steuerpfad)
- `esp32_bridge/` fuer Debug-Beobachtung per ESP-NOW-Mitlesen und MQTT-Weiterleitung an Pi (Entwicklungswerkzeug)
- `arduino_arm/` fuer direkte Servoansteuerung, Limits und Rampen
- `UART_FRAME_V1.md` fuer das minimale serielle Startformat zwischen Receiver und Arduino
- `espnow_imu_v1/`, `espnow_imu_v2/` — archivierte Controller-Firmware-Versionen
- `espnow_receiver_v1/`, `espnow_receiver_v2/` — archivierte Receiver-Firmware-Versionen

## Regeln

- keine Sensor-, Funk- oder Servologik stillschweigend zwischen den Teilbereichen vermischen
- die Bridge darf nicht in den Steuerpfad (Receiver → Arduino) eingreifen; der Steuerpfad muss ohne Bridge funktionieren
- Protokollaenderungen immer in `COMMUNICATION_FRAMEWORK.md` dokumentieren
- Limits und Neutralverhalten immer auch mit `SAFETY_FRAMEWORK.md` und `calibration/` abstimmen
- Controller und Receiver setzen WiFi-Kanal 1 explizit per esp_wifi_set_channel(); die Bridge bezieht den Kanal vom Router per WiFi.begin()

## Schnittstellen/Abhaengigkeiten

- bezieht Kommunikationsregeln aus `../COMMUNICATION_FRAMEWORK.md`
- bezieht Kalibrier- und Grenzwerte aus `../CALIBRATION_FRAMEWORK.md` und `../calibration/`
- bezieht Sicherheitsanforderungen aus `../SAFETY_FRAMEWORK.md`
