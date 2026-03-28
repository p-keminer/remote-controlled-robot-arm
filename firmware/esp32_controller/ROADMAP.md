# Roadmap - firmware/esp32_controller

## Ziel

Den Controller-ESP32 als robuste Quelle fuer Sensorwerte, Kalibrierbezug und Funkpakete aufbauen.

## Arbeitspakete

### Abgeschlossen

- [x] Verantwortungsbereich dokumentieren
- [x] BNO055-Einzeltest ohne Multiplexer vorbereiten und validieren
- [x] Multiplexer-Scan fuer mehrere IMUs aufsetzen und validieren
- [x] Flex-Sensor-Ausleseweg festziehen
- [x] Bench-Paketaufbereitung fuer ESP-NOW umsetzen
- [x] Dritten IMU ueber den Mux-Pfad als dritte Segmentquelle integrieren und bench-validieren
- [x] BNO055-Kalibrierungspersistenz im NVS mit Einzelkalibrierung (CAL0/CAL1/CAL2)
- [x] LED-Debugging invertiert (aus=OK, an=Problem) mit RGB GPIO48 als FAULT
- [x] Live-Sensorausfallerkennung und Flex-Sensor-Plausibilitaetspruefung
- [x] Multi-Peer ESP-NOW: Controller sendet an Receiver und Bridge gleichzeitig
- [x] WiFi-Kanal 1 per esp_wifi_set_channel() fuer Bridge-Koexistenz
- [x] ImuPaket v4 mit flags-Bitfeld (Bit 0 = Notaus, 59 Bytes)
- [x] Notaus-Toggle-Button auf GPIO21 mit Entprellung und RGB-Anzeige (orange blinkend)
- [x] LED-Schema invertiert: aus = OK, an = Problem; RGB GPIO48 fuer Notaus/Fehler

### Offen

- [ ] Referenz- und Nullpunktlogik sichtbar machen
- [ ] Bench-Paket nach drittem IMU und erster Arduino-Grundkette auf die dokumentierte Security-Baseline anheben
