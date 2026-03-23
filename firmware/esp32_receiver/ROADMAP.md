# Roadmap - firmware/esp32_receiver

## Ziel

Den Receiver-ESP32 als nachvollziehbare und sichere Funk-zu-UART-Bruecke vorbereiten.

## Arbeitspakete

- [x] Verantwortungsbereich dokumentieren
- [x] ESP-NOW-Empfangsablauf fuer den Bench-Pfad festlegen und validieren
- [x] Paketintegritaet fuer den Bench-Pfad konkretisieren
- [ ] dritten IMU ueber den Funkpfad bis zum Receiver sauber mitziehen
- [ ] minimale UART-Grundkette anhand `../UART_FRAME_V1.md` umsetzen und bench-validieren
- [ ] Security-Uplift mit Session- und Authentisierungsschicht erst danach umsetzen
- [ ] Timeout-Verhalten fuer Realbetrieb konkretisieren
- [ ] Fehler- und Neutralverhalten sauber beschreiben
