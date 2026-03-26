# Roadmap - firmware/esp32_bridge

## Ziel

Einen dedizierten Bridge-ESP32 als ESP-NOW-Mitleser aufsetzen, der die empfangenen Sensordaten per WiFi (UDP) an das Pi-Dashboard weiterleitet.

## Arbeitspakete

- [ ] ESP-NOW Empfang mit ImuPaket v3 Validierung implementieren
- [ ] WiFi STA-Verbindung zum lokalen Netzwerk aufbauen
- [ ] UDP-Weiterleitung der validierten Pakete an den Pi
- [ ] Status-LEDs: LINK (ESP-NOW aktiv), WIFI (verbunden), FAULT (Fehler)
- [ ] WiFi-Reconnect bei Verbindungsverlust
- [ ] Koexistenz ESP-NOW + WiFi auf gleichem Kanal bench-validieren
- [ ] Ausfall der Bridge gegen Steuerpfad-Stabilitaet testen
- [ ] ArduinoOTA mit Passwort-Schutz und Dual-Partition Rollback implementieren
- [ ] OTA-Logging per MQTT auf `robotarm/ota/log`
- [ ] Boot-Validierung: Rollback bei fehlendem WiFi/MQTT nach Timeout
