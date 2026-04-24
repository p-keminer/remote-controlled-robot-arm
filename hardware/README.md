# hardware

## Zweck

Dieser Ordner enthaelt die Hardwaredokumentation des Projekts ausserhalb der Firmware.

## Aktueller Stand

Der Arm-Prototyp ist aufgebaut und die Hardwarebasis ist nicht mehr nur konzeptionell:

- Controller auf Lochraster-/Perfboard
- Greifer-Eingabe aktuell als Potentiometer aus Robustheitsgruenden
- Receiver und Arduino als aktiver Steuerpfad
- Bridge als verifizierter Debugpfad
- Armwinkel auf Dashboard und ROS gemappt

Historische Breadboard- und Flex-Sensor-Dokumente bleiben als Entwicklungsherkunft erhalten, werden aber nicht mehr als aktueller Zielstand beschrieben.

## Inhalt

- `ADEEPT_ARM_PRODUCT_BASELINE.md` fuer den Hersteller-Ursprung
- `electronics/` fuer ESP-, Pin- und Verdrahtungsstand
- `assembly/` fuer den Aufbau
- `bringup/` fuer Inbetriebnahme und Strompfad
- `verkabelung/` fuer reale Leitungs- und Steckverbindungen
- `schaltplaene/` fuer Projekt- und Hersteller-Schaltplanbezug
- `mechanik/` fuer Trage- und Haltekonzepte
- `bom/` fuer Material- und Beschaffungsstand

## Regeln

- Herstellerstand, historische Bench-Staende und aktueller Prototypstand bleiben klar getrennt.
- Jede Aenderung mit Einfluss auf Sicherheit, Kalibrierung oder Kommunikation wird in den betroffenen Framework- und Fachdokumenten gespiegelt.
- Reale Nachweise laufen ueber `docs/`.

## Schnittstellen/Abhaengigkeiten

- arbeitet mit `../firmware/`, `../preparation/`, `../security/` und `../docs/`
