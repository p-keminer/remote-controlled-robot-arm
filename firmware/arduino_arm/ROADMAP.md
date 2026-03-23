# Roadmap - firmware/arduino_arm

## Ziel

Eine sichere Servoausfuehrung mit Limits, Rampen und definierter Fehlerreaktion vorbereiten.

## Arbeitspakete

- [x] Verantwortungsbereich dokumentieren
- [x] Stock-Baseline des originalen Adeept-Driver-Boards und seiner Servokanalannahmen aus dem offiziellen V4.0-Download dokumentieren
- [x] Projektentscheidung zu Stock-Funktionen dokumentieren — Potentiometer als Fallback, OLED uebernehmen, Learning/Action Memory uebernehmen, Processing vorerst behalten
- [ ] reale Zuordnung von `Servo1..Servo5` gegen den aufgebauten Arm bestaetigen
- [ ] bestaetigen, ob `D10/servo6` nur Reserve-/Testport bleibt
- [ ] Servokanalliste und Startverhalten festlegen
- [ ] dokumentieren, wie die beibehaltenen Stock-Funktionen spaeter technisch in den Projektpfad eingebunden oder abgeloest werden
- [ ] mechanische Limits pro Servo einpflegen
- [ ] Rampen- oder Schrittweitenbegrenzung vorbereiten
- [ ] Timeout- und Neutralverhalten fuer den Echtbetrieb konkretisieren
