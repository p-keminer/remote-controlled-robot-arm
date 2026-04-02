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
- [x] ImuPaket v4: flags-Bitfeld (Bit 0 = Notaus), 59 Bytes, auf allen drei ESPs validiert
- [x] Multi-Peer ESP-NOW: Controller sendet an Receiver und Bridge gleichzeitig
- [x] Bridge-ESP32 Firmware mit ESP-NOW Empfang, MQTT-Weiterleitung, OTA
- [x] WiFi-Kanal 1 auf allen ESPs fuer ESP-NOW/WiFi-Koexistenz
- [x] LED-Schema invertiert (aus=OK, an=Problem) mit RGB GPIO48 als FAULT/Notaus
- [x] Live-Sensorausfallerkennung und Flex-Sensor-Plausibilitaetspruefung
- [x] Notaus-Toggle-Button am Controller (GPIO21) mit Propagierung an Receiver und Bridge
- [x] RGB-LED auf allen drei ESPs: orange blinkend bei Notaus, rot bei Fehler, aus wenn OK
- [x] Firmware-Archive gesichert (espnow_imu_v1/v2, espnow_receiver_v1/v2)

### Offen

- [ ] Funkprotokoll vom Bench-Pfad auf die dokumentierte Security-Baseline anheben
- [x] I2C-Fluss zwischen Receiver und Arduino bench-validiert (GPIO13/14, Slave 0x42, Frame V1, bestaetigt 2026-04-02)
- [x] Servoebene mit Limits, Slew-Rate (MAX_SCHRITT=1, 50°/s), Timeout 1s → Neutral bench-validiert (bestaetigt 2026-04-02)
- [ ] lokale Teil-Roadmaps mit echtem Implementierungsfortschritt mitziehen
