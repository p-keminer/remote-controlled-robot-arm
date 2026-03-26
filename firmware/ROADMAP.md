# Roadmap - firmware

## Ziel

Eine klar getrennte Firmwarebasis fuer Controller, Receiver, Arduino und Bridge schaffen, auf der spaetere Integrationsarbeit sicher aufbauen kann.

## Arbeitspakete

### Abgeschlossen

- [x] Firmwarebereiche und Verantwortungsgrenzen dokumentieren
- [x] Bench-Sensorlesezyklus fuer den Controller aufsetzen
- [x] Bench-Funkpaketfluss zwischen Controller und Receiver vorbereiten und validieren
- [x] 3x BNO055 ueber PCA9548A Mux bench-validiert (Kanaele 0/1/2)
- [x] BNO055-Kalibrierungspersistenz im NVS mit Einzelkalibrierung (CAL0/CAL1/CAL2)
- [x] ImuPaket v3 mit drei IMUs, Kalibrierungsstatus, XOR-Pruefsumme und Frische-Check
- [x] Multi-Peer ESP-NOW: Controller sendet an Receiver und Bridge gleichzeitig
- [x] Bridge-ESP32 Firmware mit ESP-NOW Empfang, MQTT-Weiterleitung, OTA
- [x] WiFi-Kanal 6 auf allen ESPs fuer ESP-NOW/WiFi-Koexistenz
- [x] LED-Schema invertiert (aus=OK, blinken=Problem) mit RGB GPIO48 als FAULT
- [x] Live-Sensorausfallerkennung und Flex-Sensor-Plausibilitaetspruefung
- [x] Firmware-Archive gesichert (espnow_imu_v1/v2, espnow_receiver_v1/v2)

### Offen

- [ ] Funkprotokoll vom Bench-Pfad auf die dokumentierte Security-Baseline anheben
- [ ] UART-Fluss zwischen Receiver und Arduino implementierbar vorbereiten
- [ ] Servoebene mit Limits, Rampen und Timeout-Verhalten konkretisieren
- [ ] lokale Teil-Roadmaps mit echtem Implementierungsfortschritt mitziehen
