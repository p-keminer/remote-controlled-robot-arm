# First Power On Checklist

## Vorbereitung

- [x] Materialstand gegen Inventar geprueft
- [x] Verkabelung dokumentiert (siehe `hardware/electronics/SCHEMATIC_CURRENT.md`)
- [x] keine losen Verbindungen sichtbar
- [x] Safety- und Security-Vorbedingungen geprueft
- [x] Bench-Aufbau vorbereitet

## Erstes Einschalten — Arduino/Arm (2026-03-24)

- [x] Spannungsquelle korrekt (2x Molicel INR-18650-M35A im Battery Holder)
- [x] keine unerwartete Erwaermung
- [x] keine unerwartete Bewegung (Servos auf 90° Neutral)
- [x] serielle Erreichbarkeit dokumentiert (Arduino UNO R3 per Micro USB)
- [x] offene Auffaelligkeiten notiert: Shoulder-Servo defekt, durch Reserve ersetzt

## Erstes Einschalten — ESP32 Controller (2026-03-22)

- [x] USB-C Verbindung stabil
- [x] Board als `esp32:esp32:robotic_arm_s3n16r8` erkannt
- [x] serielle Ausgabe funktioniert
- [x] RGB-LED auf GPIO48 ansprechbar

## Erstes Einschalten — ESP32 Receiver (2026-03-22)

- [x] USB-C Verbindung stabil
- [x] ESP-NOW Empfang vom Controller funktioniert
- [x] serielle Ausgabe funktioniert
- [x] RGB-LED auf GPIO48 ansprechbar

## Erstes Einschalten — ESP32 Bridge (2026-03-26)

- [x] USB-C Verbindung stabil
- [x] WiFi-Verbindung zum lokalen Netz hergestellt (Kanal 1)
- [x] ESP-NOW Empfang vom Controller funktioniert
- [x] MQTT-Weiterleitung an Mosquitto auf Pi funktioniert

## Freigabe / Abbruch

- Ergebnis: Alle vier Geraete erfolgreich in Betrieb genommen
- Naechster erlaubter Schritt: I2C-Kette Receiver → Arduino, Servo-Sweep-Test
- Offene Risiken: Kein Reserve-Servo mehr verfuegbar
