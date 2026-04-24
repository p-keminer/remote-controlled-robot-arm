# Wiring Log

> Historischer Bench-Stand: Diese Datei dokumentiert den Wiring-Zustand der I2C-Benchphase vom 2026-04-02.
> Der aktuelle Prototyp-Wiring-Stand wird getrennt in `WIRING_LOG_2026-04-24.md` gepflegt.

## Sitzung

- Datum: 2026-04-02
- Bearbeiter: p-keminer
- Aufbauzustand: Bench-Prototyp mit I2C-Kette Receiver → Arduino validiert
- Nachtrag 2026-04-22: aktuelle Wearable-Zuordnung S0 = Hand/Wrist, S1 = Unterarm, S2 = Oberarm; reale Controller-LED-Farben GPIO4 blau, GPIO5 rot, GPIO6 weiss, GPIO7 gruen

## Verbindung — I2C (Receiver → Arduino)

| Von | Nach | Signal / Zweck | Erwartung | Tatsaechlicher Stand | Hinweis |
| --- | --- | --- | --- | --- | --- |
| ESP32 GPIO13 | Arduino A4 (SDA) | I2C SDA | bidirektional | validiert | ueber OLED-Header Pin 3 |
| ESP32 GPIO14 | Arduino A5 (SCL) | I2C SCL | Master-Clock | validiert | ueber OLED-Header Pin 4 |
| ESP32 GND | Arduino GND | gemeinsame Masse | 0V Bezug | validiert | OLED-Header Pin 1 oder 2 |

## Verbindung — Controller Sensorik

| Von | Nach | Signal / Zweck | Erwartung | Tatsaechlicher Stand | Hinweis |
| --- | --- | --- | --- | --- | --- |
| ESP32 GPIO8 | PCA9548A SDA | I2C SDA (Mux) | bidirektional | validiert | Pull-Ups auf Mux-Board |
| ESP32 GPIO9 | PCA9548A SCL | I2C SCL (Mux) | Master-Clock | validiert | Pull-Ups auf Mux-Board |
| PCA9548A CH0 | BNO055 #0 (Hand/Wrist) | I2C Mux-Kanal | Euler-Daten | validiert | Adresse 0x29 |
| PCA9548A CH1 | BNO055 #1 (Unterarm) | I2C Mux-Kanal | Euler-Daten | validiert | Adresse 0x29 |
| PCA9548A CH2 | BNO055 #2 (Oberarm) | I2C Mux-Kanal | Euler-Daten | validiert | Adresse 0x29 |
| ESP32 GPIO1 | Flex-Sensor + 10kOhm | ADC1 | Spannung 0-3.3V | validiert | gerade=1108, gebogen=940 |

## Verbindung — Controller LEDs

| Von | Nach | Signal / Zweck | Erwartung | Tatsaechlicher Stand | Hinweis |
| --- | --- | --- | --- | --- | --- |
| ESP32 GPIO4 | LED Blau + 100 Ohm | Oberarm S2 | aus=OK, an=Problem | validiert | |
| ESP32 GPIO5 | LED Rot + 100 Ohm | Unterarm S1 | aus=OK, an=Problem | validiert | |
| ESP32 GPIO6 | LED Weiss + 100 Ohm | Hand/Wrist S0 | aus=OK, an=Problem | validiert | |
| ESP32 GPIO7 | LED Gruen + 100 Ohm | COMMS | aus=OK, an=Problem | validiert | |
| ESP32 GPIO10 | LED FAULT + 100 Ohm | FAULT | aus=OK, an=Problem | validiert | Farbe separat nicht neu verifiziert |
| ESP32 GPIO21 | Taster nach GND | Notaus-Toggle | interner Pull-Up, 50ms Entprellung | validiert | |

## Verbindung — Receiver LEDs

| Von | Nach | Signal / Zweck | Erwartung | Tatsaechlicher Stand | Hinweis |
| --- | --- | --- | --- | --- | --- |
| ESP32 GPIO4 | LED Gruen + 100 Ohm | I2C | aus=OK, an=Problem | validiert | aktive I2C-Variante |
| ESP32 GPIO5 | LED Blau + 100 Ohm | ESP-NOW / LINK | aus=OK, an=Problem | validiert | blinkt bei Empfangs-Timeout |
| ESP32 GPIO48 | RGB onboard | FAULT / Notaus | aus=OK, blinkt bei Fehler | validiert | rot bei Fehler, orange bei Notaus |

## Verbindung — Arduino Servos

| Von | Nach | Signal / Zweck | Erwartung | Tatsaechlicher Stand | Hinweis |
| --- | --- | --- | --- | --- | --- |
| Arduino D9 | Servo Base | PWM 50Hz | 12-139° | validiert | Poti A0 |
| Arduino D6 | Servo Gripper | PWM 50Hz | 32-126° | validiert | Poti A1, ACHTUNG: Adeept-Standard waere Shoulder |
| Arduino D5 | Servo Wrist | PWM 50Hz | 5-177° | validiert | Poti A2, ACHTUNG: Adeept-Standard waere Elbow |
| Arduino D3 | Servo Elbow | PWM 50Hz | 80-175° | validiert | Poti A3, ACHTUNG: Adeept-Standard waere Wrist |
| Arduino D11 | Servo Shoulder | PWM 50Hz | 35-142° | validiert | Poti A6, ACHTUNG: Adeept-Standard waere Gripper |

## Stromversorgung

- Quelle: 2x Molicel INR-18650-M35A im Adeept Battery Holder (Flat Top)
- Ziel: Adeept Driver Board → Servos + Arduino Logik
- Besonderheiten: USB 500mA reicht nicht fuer 5 Servos bei permanentem 50Hz PWM — Batteriebetrieb Pflicht

## Risiko- oder Fehlerhinweise

- OLED muss ausgesteckt bleiben solange die I2C-Verbindung zum ESP32 ueber den OLED-Header laeuft (Bus-Kollision)
- Servo-Pin-Zuordnung weicht vom Adeept-Standard ab — bei Firmware-Aenderungen immer gegen diese Tabelle pruefen
- Flex-Sensor-Kabel bei Wearable-Montage nicht neben Servo-/Motorleitungen fuehren (analoges Signal, kleines Messfenster)
- I2C-Kabel (IMU/Mux) moeglichst verdrillt fuehren (SDA+GND, SCL+GND)

## Nachweise

- Foto: `docs/photos/2026-03-26_bench_aufbau_i2c_detail.jpg`
- Schaltplanbezug: `hardware/electronics/SCHEMATIC_CURRENT.md`
