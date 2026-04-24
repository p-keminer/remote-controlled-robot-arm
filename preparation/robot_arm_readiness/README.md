# preparation/robot_arm_readiness

## Zweck

Dieser Ordner beschreibt, welche Voraussetzungen fuer den aktuellen Prototypbetrieb und fuer spaetere Freigaben gelten.

## Aktueller Stand

Der Arm-Prototyp ist aufgebaut, die Sensorik ist auf Dashboard und ROS gemappt und der Debugpfad wurde verifiziert.
Die Readiness-Frage lautet daher nicht mehr nur "laesst sich das System ueberhaupt aufbauen?", sondern "welcher Teil ist fuer welchen Zweck wirklich freigegeben?".

Aktuell freigegeben fuer Entwicklung:

- Prototypbetrieb mit Controller, Receiver, Arduino und Bridge
- Dashboard- und ROS-Twin fuer Beobachtung und Mapping
- Greifer-Eingabe per Potentiometer

Noch nicht freigegeben:

- formale Safety-Freigabe fuer Produktivbewegung
- Security-Uplift des Funkpfads
- Langzeit- und Ausfallnachweise

## Regeln

- Debug- und Twin-Erfolg ersetzen keine Produktivfreigabe.
- Sicherheits- und Strompfadfragen bleiben sichtbar offen, bis dated Nachweise vorliegen.
- Neue Reifegrade muessen gleichzeitig in Hardware-, Firmware-, Test- und Security-Doku gespiegelt werden.

## Schnittstellen/Abhaengigkeiten

- `../../hardware/bringup/README.md`
- `../../tests/`
- `../../SAFETY_FRAMEWORK.md`
- `../../security/`
