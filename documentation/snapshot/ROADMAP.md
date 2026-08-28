# Roadmap

Diese Datei enthaelt nur noch offene oder dauerhaft relevante Arbeit. Erledigte
Einzelschritte bleiben ueber die Git-Historie und [docs/TESTING.md](docs/TESTING.md)
nachvollziehbar.

## Safety und reale Armfreigabe

- [ ] Hardware-Notaus beziehungsweise sichere Leistungsabschaltung definieren
- [ ] I2C-Pull-up- und Idle-Pegel zwischen ESP32-S3 und ATmega328P messen
- [ ] Timeout-, Neutral- und Recovery-Verhalten am realen Arm formal testen
- [ ] Dauerlauf-, Kollisions- und Ausfalltests dokumentieren
- [ ] entscheiden, ob Live-I2C jemals Repo-Default werden darf

## Security

- [ ] ESP-NOW-Verschluesselung und Schluessel-Provisioning festlegen
- [ ] Receiver auf erlaubte Sender-MAC beziehungsweise authentisierte Pakete begrenzen
- [ ] MQTT-, MCP- und OTA-Rechte auf notwendige Debugfunktionen reduzieren
- [ ] produktive Secrets und Geraeteidentitaeten reproduzierbar provisionieren

## Reproduzierbare Builds

- [ ] getestete ESP32-Core-Version nach Hardwaretest verbindlich pinnen
- [ ] fehlende Custom-Board-Definition `robotic_arm_s3n16r8` reproduzierbar ablegen
- [ ] Bash-Zeilenenden per `.gitattributes` fuer Windows-Checkouts festlegen
- [ ] Controller, Receiver, Bridge und Arduino in CI kompilieren
- [ ] PlatformIO-Konfigurationen gegen den Arduino-Hauptpfad angleichen

## Mapping und Digital Twin

- [ ] Mapping-Drift zwischen Realarm, Dashboard und ROS fachlich entscheiden
- [ ] gemeinsame Golden-Testvektoren fuer Realarm und Twins erstellen
- [ ] dokumentierte Neutral-/Referenzpose am realen Aufbau erneut bestaetigen
- [ ] ROS-/Dashboard-Diagnosefehler und Abhaengigkeiten separat pruefen

## Hardware und Betrieb

- [ ] aktuelle Stromversorgungs- und Sicherungswerte am finalen Aufbau messen
- [ ] Stock-/Learning-Mode des Adeept-Kits gegen den Projektstand abgrenzen
- [ ] Wearable und Perfboard fuer wiederholbare Montage kennzeichnen

## Spaeter

- [ ] mobile Plattform und Kamerasicht nur als getrennte Erweiterungen planen
- [ ] Aktuatorabstraktion fuer andere Armhardware entwerfen
