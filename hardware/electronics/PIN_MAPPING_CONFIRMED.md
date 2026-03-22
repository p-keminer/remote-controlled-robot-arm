# Pin Mapping Confirmed

Dieses Dokument zeigt ausschliesslich **bestaetigte, real getestete** GPIO-Belegungen.
Planungsannahmen und Recherchegrundlagen stehen in `PIN_MAPPING_RESEARCH.md`.

## Controller (ESP32-S3-WROOM-1-N16R8) — Stand 2026-03-22

| GPIO | Funktion | Bestaetigt | Anmerkung |
| --- | --- | --- | --- |
| GPIO8 | I2C SDA | ja | PCA9548A + BNO055, funktioniert mit Wire.begin(8,9) |
| GPIO9 | I2C SCL | ja | PCA9548A + BNO055, funktioniert mit Wire.begin(8,9) |
| GPIO1 | ADC1 Flex-Sensor | ja | Spannungsteiler 10kOhm Pull-Down, Rohwert gerade=1108 gebogen=940 |
| GPIO4 | LED Oberarm-IMU | nein | geplant, noch nicht getestet |
| GPIO5 | LED Unterarm-IMU | nein | geplant, noch nicht getestet |
| GPIO6 | LED Hand/Wrist-IMU | nein | geplant, noch nicht getestet |
| GPIO7 | LED COMMS | nein | optional, noch nicht getestet |
| GPIO10 | LED FAULT | nein | optional, noch nicht getestet |
| GPIO21 | Buzzer | nein | geplant, noch nicht getestet |

## Receiver (ESP32-S3-WROOM-1-N16R8) — Stand 2026-03-22

| GPIO | Funktion | Bestaetigt | Anmerkung |
| --- | --- | --- | --- |
| — | ESP-NOW (WiFi intern) | ja | Unicast, ImuPaket v1, Pruefsumme und Frische-Check |
| GPIO4 | LED LINK | nein | geplant, noch nicht getestet |
| GPIO5 | LED UART | nein | geplant, noch nicht getestet |
| GPIO6 | LED FAULT | nein | geplant, noch nicht getestet |
| GPIO15 | UART TX → Arduino RX | nein | geplant, noch nicht getestet |
| GPIO16 | UART RX ← Arduino TX | nein | geplant, noch nicht getestet |
| GPIO21 | Buzzer | nein | geplant, noch nicht getestet |

## Naechste Validierungsschritte

Laut `PIN_MAPPING_RESEARCH.md` Validierungsreihenfolge:

5. Sender-LEDs pruefen (GPIO4/5/6/7/10)
6. Buzzerpfad mit sicherem Default-Off pruefen (GPIO21)
7. Receiver-UART auf GPIO15/GPIO16 pruefen
8. Mux- und Mehr-IMU-Pfad erweitern (weitere BNO055 auf Kanal 2+)
