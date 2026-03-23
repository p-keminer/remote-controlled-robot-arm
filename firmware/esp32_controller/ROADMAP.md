# Roadmap - firmware/esp32_controller

## Ziel

Den Controller-ESP32 als robuste Quelle fuer Sensorwerte, Kalibrierbezug und Funkpakete aufbauen.

## Arbeitspakete

- [x] Verantwortungsbereich dokumentieren
- [x] BNO055-Einzeltest ohne Multiplexer vorbereiten und validieren
- [x] Multiplexer-Scan fuer mehrere IMUs aufsetzen und validieren
- [x] Flex-Sensor-Ausleseweg festziehen
- [x] Bench-Paketaufbereitung fuer ESP-NOW umsetzen
- [ ] dritten IMU ueber den Mux-Pfad als dritte Segmentquelle integrieren
- [ ] Referenz- und Nullpunktlogik sichtbar machen
- [ ] Bench-Paket nach drittem IMU und erster Arduino-Grundkette auf die dokumentierte Security-Baseline anheben
- [ ] LED- und Buzzer-Debugpfade einbauen
