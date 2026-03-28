# Roadmap - firmware/esp32_receiver

## Ziel

Den Receiver-ESP32 als nachvollziehbare und sichere Funk-zu-UART-Bruecke vorbereiten.

## Arbeitspakete

### Abgeschlossen

- [x] Verantwortungsbereich dokumentieren
- [x] ESP-NOW-Empfangsablauf fuer den Bench-Pfad festlegen und validieren
- [x] Paketintegritaet fuer den Bench-Pfad konkretisieren
- [x] Dritten IMU ueber den Funkpfad bis zum Receiver sauber mitgezogen und bench-validiert
- [x] WiFi-Kanal 1 Koexistenz mit Bridge validiert
- [x] LED-Schema invertiert (aus=OK, blinken=Problem) mit RGB GPIO48 als FAULT

### Offen

- [ ] Minimale UART-Grundkette anhand `../UART_FRAME_V1.md` umsetzen und bench-validieren
- [ ] Security-Uplift mit Session- und Authentisierungsschicht erst danach umsetzen
- [ ] Timeout-Verhalten fuer Realbetrieb konkretisieren
- [ ] Fehler- und Neutralverhalten sauber beschreiben
