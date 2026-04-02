# Pin Mapping Confirmed

Dieses Dokument zeigt ausschliesslich **bestaetigte, real getestete** GPIO-Belegungen.
Planungsannahmen und Recherchegrundlagen stehen in `PIN_MAPPING_RESEARCH.md`.

**LED-Schema:** Invertiert — aus = OK, an = Problem. RGB auf GPIO48 als FAULT auf allen ESPs.

## Controller (ESP32-S3-WROOM-1-N16R8) — Stand 2026-03-28

| GPIO | Funktion | Bestaetigt | Anmerkung |
| --- | --- | --- | --- |
| GPIO8 | I2C SDA | ja | PCA9548A + BNO055, funktioniert mit Wire.begin(8,9) |
| GPIO9 | I2C SCL | ja | PCA9548A + BNO055, funktioniert mit Wire.begin(8,9) |
| GPIO1 | ADC1 Flex-Sensor | ja | Spannungsteiler 10kOhm Pull-Down, Rohwert gerade=1108 gebogen=940, Live-Plausibilitaetspruefung (200-3800) |
| GPIO4 | LED Gruen (Hand/Wrist S2) | ja | 100 Ohm, blinkt wenn S2 nicht kalibriert |
| GPIO5 | LED Gelb (Unterarm S1) | ja | 100 Ohm, blinkt wenn S1 nicht kalibriert |
| GPIO6 | LED Rot (Oberarm S0) | ja | 100 Ohm, blinkt wenn S0 nicht kalibriert |
| GPIO10 | LED Weiss (FAULT) | ja | Leuchtet bei Fehler, 100 Ohm Vorwiderstand |
| GPIO7 | LED Blau (COMMS) | ja | 100 Ohm, blinkt wenn ESP-NOW Send fehlschlaegt |
| GPIO21 | Notaus-Schalter | ja | Toggle-Button nach GND, INPUT_PULLUP, 50ms Entprellung, toggelt Notaus |
| GPIO48 | LED RGB onboard (FAULT) | ja | NeoPixel, rot blinkend bei Sensorausfall/Flex-Fehler, orange blinkend bei Notaus |

## Receiver (ESP32-S3-WROOM-1-N16R8) — Stand 2026-04-02

| GPIO | Funktion | Bestaetigt | Anmerkung |
| --- | --- | --- | --- |
| — | ESP-NOW (WiFi intern, Kanal 1) | ja | Unicast, ImuPaket v4, drei IMUs + KalibStatus + Notaus-Flag, Pruefsumme und Frische-Check |
| GPIO4 | LED Gruen (I2C) | ja | 100 Ohm, leuchtet bei I2C-Frame-Aktivitaet zum Arduino |
| GPIO5 | LED Blau (ESP-NOW) | ja | 100 Ohm, blinkt wenn ESP-NOW Empfangs-Timeout (>2s) |
| GPIO48 | LED RGB onboard (FAULT) | ja | NeoPixel, rot blinkend bei Fehler/Timeout, orange blinkend bei Notaus |
| GPIO13 | I2C SDA → Arduino A4 | ja | Wire Master, 100kHz, Frame V1 (11 Bytes) an Slave 0x42 (bestaetigt 2026-04-02) |
| GPIO14 | I2C SCL → Arduino A5 | ja | Wire Master, 100kHz (bestaetigt 2026-04-02) |

## Bridge (ESP32-S3-WROOM-1-N16R8) — Stand 2026-03-28

| GPIO | Funktion | Bestaetigt | Anmerkung |
| --- | --- | --- | --- |
| — | WiFi STA (Kanal 1) | ja | Verbindung zum Router, MQTT an Mosquitto auf Pi |
| — | ESP-NOW Empfang (Kanal 1) | ja | Empfaengt ImuPaket v4 vom Controller als 2. Peer |
| GPIO4 | LED Gruen (WiFi) | ja | 100 Ohm, blinkt wenn WiFi getrennt |
| GPIO5 | LED Blau (ESP-NOW) | ja | 100 Ohm, blinkt wenn ESP-NOW Timeout (>2s) |
| GPIO7 | LED Weiss (MQTT) | ja | 100 Ohm, blinkt wenn MQTT getrennt |
| GPIO48 | LED RGB onboard (FAULT/NOTAUS) | ja | NeoPixel, orange blinkend bei Notaus (hoechste Prio), rot blinkend bei Fehler, aus wenn OK |

## WiFi-Kanal

Alle drei ESPs muessen auf dem gleichen WiFi-Kanal laufen (aktuell Kanal 1 = Router-Kanal).
Controller und Receiver setzen den Kanal per `esp_wifi_set_channel(1)`.
Die Bridge uebernimmt den Kanal automatisch bei WiFi-Verbindung zum Router.

## Naechste Validierungsschritte

- PPS-Optimierung bei Multi-Peer ESP-NOW (aktuell ~1-2 PPS statt 200)
- IMU-Daten auf Servo-Zielwinkel mappen (aktuell nur Sweep-Test)
