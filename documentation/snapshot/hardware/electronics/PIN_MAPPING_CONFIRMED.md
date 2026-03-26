# Pin Mapping Confirmed

Dieses Dokument zeigt ausschliesslich **bestaetigte, real getestete** GPIO-Belegungen.
Planungsannahmen und Recherchegrundlagen stehen in `PIN_MAPPING_RESEARCH.md`.

## Controller (ESP32-S3-WROOM-1-N16R8) — Stand 2026-03-26

| GPIO | Funktion | Bestaetigt | Anmerkung |
| --- | --- | --- | --- |
| GPIO8 | I2C SDA | ja | PCA9548A + BNO055, funktioniert mit Wire.begin(8,9) |
| GPIO9 | I2C SCL | ja | PCA9548A + BNO055, funktioniert mit Wire.begin(8,9) |
| GPIO1 | ADC1 Flex-Sensor | ja | Spannungsteiler 10kOhm Pull-Down, Rohwert gerade=1108 gebogen=940, Live-Plausibilitaetspruefung (200-3800) |
| GPIO4 | LED Gruen (Hand/Wrist S2) | ja | 100 Ohm Vorwiderstand, an wenn Sensor bereit + Gyro kalibriert |
| GPIO5 | LED Gelb (Unterarm S1) | ja | 100 Ohm Vorwiderstand, an wenn Sensor bereit + Gyro kalibriert |
| GPIO6 | LED Rot (Oberarm S0) | ja | 100 Ohm Vorwiderstand, an wenn Sensor bereit + Gyro kalibriert |
| GPIO7 | LED Blau (COMMS) | ja | 100 Ohm Vorwiderstand, an bei erfolgreichem ESP-NOW Senden |
| GPIO10 | LED Weiss (FAULT) | ja | 100 Ohm Vorwiderstand, an bei IMU- oder Flex-Sensor-Ausfall |
| GPIO21 | Buzzer | nein | entfaellt vorerst, LEDs reichen fuer Debugging |

## Receiver (ESP32-S3-WROOM-1-N16R8) — Stand 2026-03-26

| GPIO | Funktion | Bestaetigt | Anmerkung |
| --- | --- | --- | --- |
| — | ESP-NOW (WiFi intern) | ja | Unicast, ImuPaket v3, drei IMUs + KalibStatus, Pruefsumme und Frische-Check |
| GPIO4 | LED Gruen (LINK) | ja | 100 Ohm Vorwiderstand, an bei erfolgreichem Paketempfang |
| GPIO5 | LED Blau (UART) | ja | 100 Ohm Vorwiderstand, reserviert fuer spaetere UART-Weiterleitung |
| GPIO6 | LED Gelb (FAULT) | ja | 100 Ohm Vorwiderstand, an bei Fehler oder Empfangs-Timeout (>2s) |
| GPIO15 | UART TX → Arduino RX | nein | geplant, noch nicht getestet |
| GPIO16 | UART RX ← Arduino TX | nein | geplant, noch nicht getestet |
| GPIO21 | Buzzer | nein | entfaellt vorerst, LEDs reichen fuer Debugging |

## Naechste Validierungsschritte

7. Receiver-UART auf GPIO15/GPIO16 pruefen
