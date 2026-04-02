# Bench Test Record — I2C-Kette Receiver → Arduino

## Testfall

- Datum: 2026-04-02
- Bearbeiter: p-keminer
- Testbereich: Kommunikation Receiver → Arduino, Servo-Steuerung
- Ziel: I2C-Grundkette end-to-end validieren (ESP32 → Arduino → 5 Servos)

## Voraussetzungen

- ESP32 Receiver mit `esp32_receiver_i2c.ino` geflasht
- Arduino mit `arduino_arm_i2c.ino` geflasht
- I2C-Verbindung: ESP32 GPIO13 (SDA) → Arduino A4, ESP32 GPIO14 (SCL) → Arduino A5 ueber OLED-Header
- Gemeinsame Masse (GND) zwischen ESP32 und Arduino
- Batteriebetrieb (2x Molicel INR-18650-M35A im Adeept Battery Holder)
- OLED Display ausgesteckt (belegt gleichen I2C-Bus)

## Ablauf

1. I2C-Frame V1 (11 Bytes) per `i2c_frame_senden()` vom Receiver an Arduino (Slave 0x42) gesendet
2. Arduino empfaengt per `Wire.onReceive()` — ISR kopiert nur Rohbytes in Puffer
3. Validierung in `loop()`: Startbytes (0xA5/0x5A), Protokollversion, CRC8
4. Gueltige Frames: map() der 5 Achswerte auf Servo-Limits, Slew-Rate-Limiter anwenden
5. Sweep-Test (`test_sweep_i2c.h`): Pendelbewegung ueber alle 5 Achsen gleichzeitig

## Ergebnis

- Erwartetes Verhalten: Alle 5 Servos bewegen sich synchron im Pendel, kein Jitter
- Tatsaechliches Verhalten: Alle 5 Servos laufen smooth ohne Jitter, Slew-Rate-Limiter begrenzt auf 50°/s
- Messwerte:
  - I2C-Update-Rate: 50Hz (20ms Intervall)
  - Slew-Rate: MAX_SCHRITT=1 pro 20ms Zyklus = 50°/s
  - Timeout: 1s ohne gueltigen Frame → alle Achsen auf Neutral (90°)
  - Servo-Limits: Base 12-139°, Shoulder 35-142°, Elbow 80-175°, Wrist 5-177°, Gripper 32-126°

## Sicherheits- und Security-Hinweise

- ISR-minimales Design kritisch: map()-Berechnungen im ISR verursachten Servo-Zucken (32-Bit Arithmetik auf 8-Bit AVR stoert Timer1-PWM)
- Servos dauerhaft attached — kein Detach-Zyklus noetig dank TWI-Hardware (kein Konflikt mit Timer1)
- Batteriebetrieb Pflicht: USB 500mA reicht nicht fuer 5 Servos bei permanentem 50Hz PWM

## Naechster Schritt

- Security-Uplift mit Session- und Authentisierungsschicht
- LED-Pfade gegen bestaetigten Pinplan validieren (Buzzer gestrichen)
