# Roadmap - firmware/esp32_receiver

## Ziel

Den Receiver-ESP32 als nachvollziehbare und sichere Funk-zu-I2C-Bruecke vorbereiten.

## Arbeitspakete

### Abgeschlossen

- [x] Verantwortungsbereich dokumentieren
- [x] ESP-NOW-Empfangsablauf fuer den Bench-Pfad festlegen und validieren
- [x] Paketintegritaet fuer den Bench-Pfad konkretisieren
- [x] Dritten IMU ueber den Funkpfad bis zum Receiver sauber mitgezogen und bench-validiert
- [x] WiFi-Kanal 1 Koexistenz mit Bridge validiert
- [x] LED-Schema invertiert (aus=OK, an=Problem) mit RGB GPIO48 als FAULT
- [x] ImuPaket v4 Empfang mit flags-Bitfeld (59 Bytes)
- [x] Notaus-Flag-Empfang mit RGB-Anzeige (orange blinkend bei Notaus)

### Offen

- [x] I2C-Grundkette anhand `../UART_FRAME_V1.md` umgesetzt und bench-validiert (bestaetigt 2026-04-02)
- [ ] Security-Uplift mit Session- und Authentisierungsschicht erst danach umsetzen
- [ ] Timeout-Verhalten fuer Realbetrieb konkretisieren
- [ ] Fehler- und Neutralverhalten sauber beschreiben
