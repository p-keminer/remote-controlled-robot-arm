# CLAUDE.md

Dieses Dokument dient als schneller Projektbrief fuer KI-Systeme, die ohne tieferen Verlauf in dieses Repository kommen.

## Was dieses Projekt ist

`IMU Robotic Arm` ist ein dokumentationsgetrieben vorbereitetes Embedded-Projekt fuer einen 5-DOF-Roboterarm.
Der Kern des Systems besteht aus mehreren grossen Saeulen:

- Wearable-Controller mit IMUs und Flex-Sensor
- lokaler Funkpfad ueber `ESP-NOW`
- Receiver-ESP32 als Bruecke zur Servoebene
- Arduino-basierte Servoausfuehrung
- getrennte Sicherheits-, Kalibrierungs- und Hardwaredokumentation
- nachvollziehbare Vorbereitung fuer Toolchain, Bench und spaetere Inbetriebnahme

## Was dieses Projekt noch nicht ist

- noch kein zusammengebauter Endaufbau
- noch kein fertig eingerichteter ESP32-Workflow
- noch keine implementierte Firmwarekette
- kein WLAN-, Cloud- oder Web-Interface-Projekt

## Aktuell verbindliche Grundentscheidungen

- `ESP-NOW` ist der Funkstandard fuer v1.
- WLAN ist in v1 ausdruecklich nicht der Zielpfad.
- `Security` und `Safety` werden getrennt dokumentiert.
- Der reale Aufbau wird in dieser Ausbaustufe nur als Ablauf, Checklisten- und Nachweisstruktur dokumentiert.
- Nach jedem groesseren Schritt muessen die betroffenen Dokumente aktualisiert und anschliessend `bash ./scripts/update_docs.sh` ausgefuehrt werden.

## Wichtige Dateien zuerst lesen

- `README.md`
- `PROJEKT_ABLAUFPLAN.md`
- `PROJEKT_FORTSCHRITT.md`
- `GLOBAL_RULES.md`
- `ARCHITECTURE.md`
- `SYSTEM_FRAMEWORKS.md`
- `COMMUNICATION_FRAMEWORK.md`
- `SECURITY_FRAMEWORK.md`
- `CALIBRATION_FRAMEWORK.md`
- `SAFETY_FRAMEWORK.md`
- `preparation/README.md`
- `security/README.md`
- `hardware/README.md`

## Grundsatz

Jede neue Implementierung oder Dokumentation soll das Projekt besser strukturieren, sicherer machen und spaeter leichter auf andere Hardware uebertragbar halten.
