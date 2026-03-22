# hardware

## Zweck

Dieser Ordner enthaelt alle nicht-firmwarebezogenen Hardwareunterlagen des Projekts.

## Aktueller Stand

Die Hardwarestruktur ist dokumentiert und wurde um Aufbauablauf, Bringup, Wearable-Konzept und Elektronikbench erweitert.
Schaltplanstand, Verkabelung, Mechanik, Elektronik, Aufbau, Bringup und Stueckliste sind bewusst getrennt; reale Inventar- und Aufbau-Nachweise fehlen noch.

## Inhalt

- `schaltplaene/` fuer elektrische Schaltunterlagen
- `verkabelung/` fuer reale Verbindungsplaene und Kabelwege
- `mechanik/` fuer Montage, Halterungen und Befestigung
- `electronics/` fuer ESP-Integration, Pinmapping, Breadboard-/Lochrasterpfad und Debugsignale
- `assembly/` fuer den dokumentierten Aufbauablauf
- `bringup/` fuer Vorpruefungen und sichere Erstinbetriebnahme
- `bom/` fuer Stuecklisten und Materialstand

## Regeln

- theoretische Schaltplaene, dokumentierten Aufbauablauf und reale Verdrahtung nicht vermischen
- mechanische Abweichungen vom Plan dokumentieren
- Hardwareentscheide, die Kalibrierung oder Sicherheit beeinflussen, in den passenden Frameworks spiegeln
- hardwarebezogene Konzept- und Entscheidungsdokumente muessen einen Abschnitt `Recherchequellen` mit den verwendeten Referenzen enthalten
- reale Nachweise spaeter ueber `docs/templates/` und `docs/` fuehren

## Schnittstellen/Abhaengigkeiten

- referenziert Sicherheitsannahmen aus `../SAFETY_FRAMEWORK.md`
- referenziert Security-Annahmen aus `../SECURITY_FRAMEWORK.md`
- referenziert Kalibrierungsbezug aus `../CALIBRATION_FRAMEWORK.md`
- liefert Nachweise und Bilder nach `../docs/`
- arbeitet mit `../preparation/robot_arm_readiness/` zusammen
