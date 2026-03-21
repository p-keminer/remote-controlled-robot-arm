# firmware/arduino_arm

## Zweck

Dieser Ordner ist fuer die direkte Servoausfuehrung auf dem Adeept-Arduino reserviert.

## Aktueller Stand

Die Rolle des Arduino ist dokumentiert, die Implementierung folgt spaeter.
Die Servoebene ist als letzter Sicherheits- und Bewegungsanker definiert.

## Inhalt

- spaetere Servoinitialisierung und Pinzuordnung
- Anwendung von Limits, Neutralwerten und Rampen
- UART-Empfang vom Receiver-ESP32
- Fehlerreaktion bei ausbleibenden oder ungueltigen Daten

## Regeln

- mechanische Limits duerfen nicht nur in externer Dokumentation existieren, sondern muessen in der Servoebene abgesichert sein
- Bewegungen werden nicht sprunghaft ohne Rampenbegrenzung ausgegeben
- Neutral- und Stopverhalten muessen mit `SAFETY_FRAMEWORK.md` und `calibration/servo_limits/` uebereinstimmen

## Schnittstellen/Abhaengigkeiten

- empfaengt validierte Zielwerte aus `../esp32_receiver/`
- nutzt Limit- und Neutralregeln aus `../../calibration/servo_limits/`
- orientiert sich an `../../SAFETY_FRAMEWORK.md`
