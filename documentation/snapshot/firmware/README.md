# firmware

## Zweck

Dieser Ordner enthaelt die gesamte Laufzeitlogik fuer Controller, Receiver, Servoausfuehrung und die Debug-Bridge.

## Aktueller Stand

Controller, Receiver und Bridge sind als Bench-Firmware vorhanden und bench-validiert.
Controller sendet ImuPaket v3 per ESP-NOW an Receiver (Steuerpfad) und Bridge (Debug-Pfad) gleichzeitig.
Alle drei ESPs laufen auf WiFi-Kanal 1 (Router-Kanal) fuer ESP-NOW/WiFi-Koexistenz.
LED-Schema ist invertiert: aus = OK, blinken = Problem, RGB auf GPIO48 = FAULT.
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

## Board- und Flash-Konfiguration

Alle drei ESP32-S3-WROOM-1-N16R8 muessen mit dem Custom Board geflasht werden:

```
FQBN: esp32:esp32:robotic_arm_s3n16r8
```

**NIEMALS** das generische Board `esp32:esp32:esp32s3` verwenden — `CDCOnBoot` ist dort deaktiviert, was bei diesen Boards einen permanenten Reset-Loop verursacht.

Die Board-Definition liegt in `boards.local.txt` im lokalen Arduino-Paketordner (siehe `../preparation/esp32_environment/README.md`).

Kritische Einstellungen: `cdc_on_boot=1`, `flash_size=16MB`, `psram_type=opi`, `partitions=app3M_fat9M_16MB`.

## Regeln

- keine Sensor-, Funk- oder Servologik stillschweigend zwischen den Teilbereichen vermischen
- die Bridge darf nicht in den Steuerpfad (Receiver → Arduino) eingreifen; der Steuerpfad muss ohne Bridge funktionieren
- Protokollaenderungen immer in `COMMUNICATION_FRAMEWORK.md` dokumentieren
- Limits und Neutralverhalten immer auch mit `SAFETY_FRAMEWORK.md` und `calibration/` abstimmen
- alle ESPs muessen auf dem gleichen WiFi-Kanal laufen (aktuell Kanal 1)

## Schnittstellen/Abhaengigkeiten

- bezieht Kommunikationsregeln aus `../COMMUNICATION_FRAMEWORK.md`
- bezieht Kalibrier- und Grenzwerte aus `../CALIBRATION_FRAMEWORK.md` und `../calibration/`
- bezieht Sicherheitsanforderungen aus `../SAFETY_FRAMEWORK.md`
