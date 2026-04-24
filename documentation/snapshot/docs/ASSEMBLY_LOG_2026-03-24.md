# Assembly Log — Stock-Aufbau

## Sitzung

- Datum: 2026-03-24
- Bearbeiter: p-keminer
- Teilabschnitt: Mechanischer Stock-Aufbau des Adeept 5-DOF Roboterarms

## Ausgangslage

- Was ist bereits montiert: nichts — Erstaufbau aus dem Kit
- Was wird in dieser Sitzung vorbereitet oder dokumentiert: kompletter mechanischer Aufbau nach Adeept V4.0 Tutorial

## Durchgefuehrte Schritte

1. Acrylteile sortiert und gegen Packing List geprueft
2. Mechanischen Aufbau nach offizieller Adeept-Anleitung durchgefuehrt
3. 5 Servos in die vorgesehenen Positionen verbaut
4. Square Bearing Turntable als Basis montiert
5. Battery Holder angebracht
6. Driver Board und OLED eingesetzt
7. Potentiometer verdrahtet
8. Fotos des fertigen Aufbaus unter `docs/photos/` abgelegt (Seitenansicht, Rueckansicht, Basisdetail)

## Beobachtungen

- Ein Servo (Original Shoulder) war defekt — durch den Reserve-/Testservo ersetzt
- Keine weiteren Reserve-Servos mehr verfuegbar nach dem Tausch
- Mechanischer Aufbau stabil, Acrylteile sauber verschraubt

## Abweichungen vom geplanten Ablauf

- Shoulder-Servo musste getauscht werden (Original defekt)
- Die reale Servo-Pin-Zuordnung weicht vom Adeept-Standard ab: D6=Gripper (nicht Shoulder), D5=Wrist (nicht Elbow), D3=Elbow (nicht Wrist), D11=Shoulder (nicht Gripper)

## Naechste Schritte

- Stock-Baseline-Test mit Batteriebetrieb durchfuehren
- Servo-Limits empirisch kalibrieren
- ESP32-Receiver per I2C an Arduino anbinden
