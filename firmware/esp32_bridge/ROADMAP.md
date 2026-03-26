# Roadmap - firmware/esp32_bridge

## Ziel

Einen dedizierten Bridge-ESP32 als ESP-NOW-Mitleser aufsetzen, der die empfangenen Sensordaten per WiFi und MQTT an das Pi-Dashboard weiterleitet.

## Arbeitspakete

### Abgeschlossen

- [x] ESP-NOW Empfang mit ImuPaket v3 Validierung (Groesse, Absender-MAC, Pruefsumme, Protokollversion)
- [x] WiFi STA-Verbindung zum lokalen Netzwerk (Kanal 6)
- [x] MQTT-Publizierung auf Topics `robotarm/imu`, `robotarm/status`, `robotarm/kalib`
- [x] Status-LEDs invertiert (aus=OK, blinken=Problem): WiFi(Gruen), ESP-NOW(Blau), MQTT(Weiss), FAULT(RGB rot)
- [x] WiFi-Reconnect bei Verbindungsverlust
- [x] Koexistenz ESP-NOW + WiFi auf Kanal 6 bench-validiert
- [x] ArduinoOTA mit Passwort-Schutz implementiert
- [x] OTA-Logging per MQTT auf `robotarm/ota/log`

### Offen

- [ ] Ausfall der Bridge gegen Steuerpfad-Stabilitaet testen (Receiver muss ohne Bridge laufen)
- [ ] Boot-Validierung: Rollback bei fehlendem WiFi/MQTT nach Timeout
- [ ] OTA Dual-Partition Rollback bench-validieren
- [ ] PPS-Optimierung (aktuell ~1-2 PPS statt 20, bedingt durch Multi-Peer Timing)
