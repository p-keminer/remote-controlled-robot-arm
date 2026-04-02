# firmware/arduino_arm

## Zweck

Dieser Ordner ist fuer die direkte Servoausfuehrung auf dem Adeept-Arduino reserviert.

## Aktueller Stand

Die Arduino-Firmware ist als I2C-Variante (`arduino_arm_i2c.ino`) bench-validiert und steuert alle 5 Servos.
Arduino ist I2C-Slave (Adresse 0x42) und empfaengt Frame V1 (11 Bytes) vom ESP32-Receiver ueber Wire-Library (SDA=A4, SCL=A5 via OLED-Header).
Servos bleiben dauerhaft attached (kein Detach-Zyklus dank TWI-Hardware statt SoftwareSerial). 50Hz Update-Rate mit Slew-Rate-Limiter (MAX_SCHRITT=1, 50 Grad/s).
ISR-minimales Design: Wire.onReceive() kopiert nur Rohbytes, Validierung und map() passieren in loop().
Servo-Limits empirisch kalibriert: Base 12-139 (D9), Gripper 32-126 (D6), Wrist 5-177 (D5), Elbow 80-175 (D3), Shoulder 35-142 (D11).
Die offizielle Produktbasis des Stock-Kits nennt das `Adeept Robotic Arm Driver Board` als `ATMEGA328P`-basierte Arduino-kompatible Steuerplatine; in der Arduino IDE ist dafuer `Arduino UNO R3` zu waehlen.
Batteriebetrieb ist erforderlich (USB 500mA reicht nicht fuer 5 Servos bei permanentem PWM).

## Inhalt

- `arduino_arm_i2c.ino` — aktive I2C-Variante: Wire-Slave 0x42, 5 Servos, ISR-minimales Design, 50Hz
- `arduino_arm.ino` — Original-UART-Variante (archiviert, nicht aktiv)
- Empirisch kalibrierte Servo-Limits pro Achse
- Slew-Rate-Limiter (MAX_SCHRITT=1, 50 Grad/s) und Timeout-Neutralverhalten (1s)
- Fehlerreaktion bei ausbleibenden oder ungueltigen Frames

## Regeln

- mechanische Limits duerfen nicht nur in externer Dokumentation existieren, sondern muessen in der Servoebene abgesichert sein
- Bewegungen werden nicht sprunghaft ohne Rampenbegrenzung ausgegeben
- Neutral- und Stopverhalten muessen mit `SAFETY_FRAMEWORK.md` und `calibration/servo_limits/` uebereinstimmen
- Herstellercode ist Referenz fuer den Stock-Zustand, aber nicht automatisch Projektzielcode
- der sechste Stock-Testport darf nicht ohne reale Aufbaupruefung als zusaetzliche Robotikachse behandelt werden

## Schnittstellen/Abhaengigkeiten

- empfaengt validierte Zielwerte per I2C (Wire-Slave 0x42) aus `../esp32_receiver/`
- Frame-Format dokumentiert in `../I2C_FRAME_V1.md`
- Designentscheidungen dokumentiert in `../SERVO_UART_DESIGNENTSCHEIDUNGEN.md` (insb. Abschnitt 13: I2C-Migration)
- nutzt Limit- und Neutralregeln aus `../../calibration/servo_limits/`
- orientiert sich an `../../SAFETY_FRAMEWORK.md`

## Recherchequellen

- [Adeept 5-DOF Robotic Arm Kit Compatible with Arduino IDE](https://www.adeept.com/adeept-5-dof-robotic-arm-kit-compatible-with-arduino-ide-programmable-diy-coding-stem-educational-5-axis-robot-arm-with-oled-display-processing-code_p0360.html) fuer den Hinweis, dass das `Adeept Robotic Arm Driver Board` auf `ATMEGA328P` basiert und in der Arduino IDE als `Arduino UNO R3` zu behandeln ist.
- [Adeept V4 Package Review](../../official_downloads/ADEEPT_V4_PACKAGE_REVIEW.md) fuer die Einordnung von `Servo90`, `Unpacking_test_code` und der offiziellen 5-vs-6-Servo-Annahmen.
- [Servo90.ino](../../official_downloads/extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Code/_6%20Servo90/Servo90/Servo90.ino) fuer die offizielle 5-Servo-90-Grad-Basis auf `D9`, `D6`, `D5`, `D3`, `D11`.
- [AdeeptArmRobot.ino](../../official_downloads/extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Code/9.%20Processing/AdeeptArmRobot/AdeeptArmRobot.ino) fuer die offizielle 5-Servo-Laufzeitbasis des Processing-Steuerpfads.
