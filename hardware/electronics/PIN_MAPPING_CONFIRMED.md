# Pin Mapping Confirmed

Dieses Dokument zeigt ausschliesslich **bestaetigte, real getestete** GPIO-Belegungen.
Planungsannahmen und Recherchegrundlagen stehen in `PIN_MAPPING_RESEARCH.md`.

**LED-Schema:** Invertiert — aus = OK, blinken = Problem. RGB auf GPIO48 als FAULT auf allen ESPs.

## Controller (ESP32-S3-WROOM-1-N16R8) — Stand 2026-03-26

| GPIO | Funktion | Bestaetigt | Anmerkung |
| --- | --- | --- | --- |
| GPIO8 | I2C SDA | ja | PCA9548A + BNO055, funktioniert mit Wire.begin(8,9) |
| GPIO9 | I2C SCL | ja | PCA9548A + BNO055, funktioniert mit Wire.begin(8,9) |
| GPIO1 | ADC1 Flex-Sensor | ja | Spannungsteiler 10kOhm Pull-Down, Rohwert gerade=1108 gebogen=940, Live-Plausibilitaetspruefung (200-3800) |
| GPIO4 | LED Gruen (Hand/Wrist S2) | ja | 100 Ohm, blinkt wenn S2 nicht kalibriert |
| GPIO5 | LED Gelb (Unterarm S1) | ja | 100 Ohm, blinkt wenn S1 nicht kalibriert |
| GPIO6 | LED Gelb (Oberarm S0) | ja | 100 Ohm, blinkt wenn S0 nicht kalibriert |
| GPIO7 | LED Blau (COMMS) | ja | 100 Ohm, blinkt wenn ESP-NOW Send fehlschlaegt |
| GPIO48 | LED RGB onboard (FAULT) | ja | NeoPixel, rot blinkend bei Sensorausfall oder Flex-Fehler |
| GPIO21 | Buzzer | nein | entfaellt vorerst, LEDs reichen fuer Debugging |

## Receiver (ESP32-S3-WROOM-1-N16R8) — Stand 2026-03-26

| GPIO | Funktion | Bestaetigt | Anmerkung |
| --- | --- | --- | --- |
| — | ESP-NOW (WiFi intern, Kanal 1) | ja | Unicast, ImuPaket v3, drei IMUs + KalibStatus, Pruefsumme und Frische-Check |
| GPIO4 | LED Gruen (UART) | ja | 100 Ohm, reserviert fuer spaetere UART-Weiterleitung |
| GPIO5 | LED Blau (ESP-NOW) | ja | 100 Ohm, blinkt wenn ESP-NOW Empfangs-Timeout (>2s) |
| GPIO48 | LED RGB onboard (FAULT) | ja | NeoPixel, rot blinkend bei Fehler oder Empfangs-Timeout |
| GPIO15 | UART TX → Arduino RX | nein | geplant, noch nicht getestet |
| GPIO16 | UART RX ← Arduino TX | nein | geplant, noch nicht getestet |

## Bridge (ESP32-S3-WROOM-1-N16R8) — Stand 2026-03-26

| GPIO | Funktion | Bestaetigt | Anmerkung |
| --- | --- | --- | --- |
| — | WiFi STA (Kanal 1) | ja | Verbindung zum Router, MQTT an Mosquitto auf Pi |
| — | ESP-NOW Empfang (Kanal 1) | ja | Empfaengt ImuPaket v3 vom Controller als 2. Peer |
| GPIO4 | LED Gruen (WiFi) | ja | 100 Ohm, blinkt wenn WiFi getrennt |
| GPIO5 | LED Blau (ESP-NOW) | ja | 100 Ohm, blinkt wenn ESP-NOW Timeout (>2s) |
| GPIO7 | LED Weiss (MQTT) | ja | 100 Ohm, blinkt wenn MQTT getrennt |
| GPIO48 | LED RGB onboard (FAULT) | ja | NeoPixel, rot blinkend bei irgendwelchem Problem |

## WiFi-Kanal

Alle drei ESPs muessen auf dem gleichen WiFi-Kanal laufen (aktuell Kanal 1 = Router-Kanal).
Controller und Receiver setzen den Kanal per `esp_wifi_set_channel(1)`.
Die Bridge uebernimmt den Kanal automatisch bei WiFi-Verbindung zum Router.

## Naechste Validierungsschritte

- Receiver-UART auf GPIO15/GPIO16 pruefen
- PPS-Optimierung bei Multi-Peer ESP-NOW (aktuell ~1-2 PPS statt 20)
