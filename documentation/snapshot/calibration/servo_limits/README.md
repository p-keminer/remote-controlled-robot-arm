# calibration/servo_limits

## Zweck

Dieser Ordner dokumentiert die mechanischen und softwareseitigen Winkelgrenzen aller Servos.

## Aktueller Stand

Die Dokumentationsbasis ist angelegt.
Reale Grenzwerte werden nach Messung und Erstinbetriebnahme eingetragen.
Die offizielle Herstellerbasis ist inzwischen klarer: Laufzeitseitig werden fuenf aktive Servos auf `D9`, `D6`, `D5`, `D3`, `D11` genutzt, waehrend `D10/servo6` als Stock-Test-/Reservepfad auftaucht.
Vor dem realen Aufbau bleiben die tatsaechliche physische Zuordnung, reale Grenzwerte und die Bestaetigung des Reserverservos trotzdem offen.

## Inhalt

- Min-/Max-Winkel pro Servo
- Neutralpositionen und sichere Rueckfallwerte
- Hinweise auf mechanische Anschlaege oder kritische Bereiche
- spaetere Zuordnung der real verbauten Servos nach Stock-Aufbau
- Abgleich zwischen offizieller Stock-5-Servo-Basis und realem Kit-Inhalt

## Regeln

- jeder Grenzwert muss auf eine reale Beobachtung, Messung oder technische Vorgabe zurueckfuehrbar sein
- Komfortgrenzen und harte Sicherheitsgrenzen nicht vermischen
- Stock-Code-Annahmen duerfen nicht direkt als reale Grenzwerte uebernommen werden

## Schnittstellen/Abhaengigkeiten

- folgt `../README.md`, `../../SAFETY_FRAMEWORK.md` und `../../CALIBRATION_FRAMEWORK.md`
- wirkt direkt auf `../../firmware/arduino_arm/`

## Recherchequellen

- [Adeept V4 Package Review](../../official_downloads/ADEEPT_V4_PACKAGE_REVIEW.md) fuer die Einordnung `5 aktive Servos + 1 Reserve/Testservo`.
- [Servo90.ino](../../official_downloads/extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Code/_6%20Servo90/Servo90/Servo90.ino) fuer die offizielle 5-Servo-Initialbasis.
