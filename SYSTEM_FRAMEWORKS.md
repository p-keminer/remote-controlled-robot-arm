# System Frameworks

Dieses Dokument beschreibt die verbindlichen Subsysteme und ihre Verantwortungen.

## Uebersicht

Das Projekt ist in folgende feste Bereiche gegliedert:

- `preparation/` fuer Toolchain, Bench und Inbetriebnahmebereitschaft
- `security/` fuer Bedrohungsmodell, Kommunikationshaertung und Provisioning-Regeln
- `firmware/esp32_controller` fuer Sensorerfassung, Vorverarbeitung und Sendevorbereitung
- `firmware/esp32_receiver` fuer Funkempfang, Validierung und Uebergabe an den Arduino
- `firmware/arduino_arm` fuer Servoansteuerung, Limits, Rampen und Fehlerverhalten
- `hardware/` fuer Schaltplaene, Verkabelung, Aufbauablauf, Bringup und Stueckliste
- `calibration/` fuer Referenzdaten, Grenzen und Mappingdokumentation
- `tests/` fuer Bench-, Security-, Integrations-, Latenz- und Safety-Nachweise
- `docs/` fuer Sitzungsnotizen, Messprotokolle, Bilder und Templates

## Preparation-Framework

- beschreibt die Voraussetzungen, bevor echte Entwicklungs- und Inbetriebnahmeschritte beginnen
- trennt Toolchain, Arbeitsplatz, Hardware-Readiness und Freigabepunkte sichtbar

## Security-Framework

- beschreibt Angreifer, Trust Boundaries, Kommunikationshaertung und Provisioning-Regeln
- trennt Security von physischer Bewegungssicherheit

## Controller-Framework

- liest IMUs und Flex-Sensor in definierter Reihenfolge
- fuehrt notwendige Vorverarbeitung und Segmentzuordnung durch
- erzeugt fachliche Zielwerte fuer Servos, aber keine direkte Servoausgabe

## Receiver-Framework

- kapselt den `ESP-NOW`-Empfang
- validiert Paketinhalte, Zeitverhalten und Integritaet
- uebersetzt Funkframes in ein einfaches, nachvollziehbares UART-Format

## Arduino-Framework

- haelt die direkte Verbindung zur Servo-Hardware
- setzt Gelenklimits, Neutralpositionen und Rampen durch
- reagiert definiert auf Datenverlust oder ungueltige Frames

## Hardware-Framework

- trennt Schaltplanstand, reale Verkabelung, Aufbauablauf, Bringup und Stueckliste
- dokumentiert Abweichungen zwischen geplanter und realer Hardwarelage

## Kalibrierungs-Framework

- trennt IMU-Referenzen, Flex-Sensor-Kennlinien, Servo-Limits und Referenzposen
- macht nachvollziehbar, aus welcher Messung welche Grenze oder Invertierung stammt

## Test-Framework

- `bench/` fuer kleine Teilpruefungen an Sensorik oder Schnittstellen
- `security/` fuer Angriffs-, Missbrauchs- und Integritaetsfaelle
- `integration/` fuer Ende-zu-Ende-Ablaufpruefungen
- `latency/` fuer Zeitverhalten und Messmethoden
- `safety/` fuer Fehlerfaelle, Grenzwertpruefung und Freigabetests
