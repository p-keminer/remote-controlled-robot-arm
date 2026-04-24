# Roadmap - firmware/arduino_arm

## Ziel

Eine sichere Servoausfuehrung mit Limits, Rampen und definierter Fehlerreaktion vorbereiten.

## Arbeitspakete

- [x] Verantwortungsbereich dokumentieren
- [x] Stock-Baseline des originalen Adeept-Driver-Boards und seiner Servokanalannahmen aus dem offiziellen V4.0-Download dokumentieren
- [x] Projektentscheidung zu Stock-Funktionen dokumentieren — Potentiometer als Fallback, OLED uebernehmen, Learning/Action Memory uebernehmen, Processing vorerst behalten
- [x] reale Zuordnung von Servo1..Servo5 gegen den aufgebauten Arm bestaetigt: Base=D9, Gripper=D6, Wrist=D5, Elbow=D3, Shoulder=D11 (weicht vom Adeept-Standard ab, empirisch ermittelt 2026-04-02)
- [x] bestaetigt: D10/servo6 bleibt Reserve-/Testport
- [x] Servokanalliste und Startverhalten festgelegt: Mittelwerte (MIN+MAX)/2 als Startposition statt pauschal 90 Grad
- [ ] dokumentieren, wie die beibehaltenen Stock-Funktionen spaeter technisch in den Projektpfad eingebunden oder abgeloest werden
- [x] mechanische Limits pro Servo empirisch kalibriert: Base 12-139, Shoulder 35-142, Elbow 80-175, Wrist 5-177, Gripper 32-126
- [x] Slew-Rate-Limiter implementiert: MAX_SCHRITT=1 bei 50Hz = 50 Grad/s
- [x] Timeout (1s) und Neutralverhalten implementiert: bei ausbleibenden Frames → Mittelposition
- [x] I2C-Empfang statt UART implementiert: Wire-Slave 0x42, ISR-minimales Design, bench-validiert (2026-04-02)
- [ ] IMU-Daten auf Servo-Zielwinkel mappen (aktuell nur Sweep-Test)
