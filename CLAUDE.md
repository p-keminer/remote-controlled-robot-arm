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

## Kein Hartcodieren in Firmware

MACs, Schluessel, Peer-Adressen und Geraete-IDs duerfen nicht direkt in `.ino`- oder `.cpp`-Dateien hartcodiert werden. Diese Werte gehoeren in gitignorierte lokale Konfigurationsdateien (`peer_config.local.h`, `credentials.local.h` o.ae.). Committed werden nur Templates oder Platzhalter-Header.

## Vor jedem Push: Secret-Pruefung

Vor jedem `git push` muss geprueft werden, ob in getrackte Dateien Secrets, Schluessel, MACs, IP-Adressen, Passwoerter oder andere sensible Werte eingeflossen sind, die nicht durch `.gitignore` geblockt werden. Der Push darf erst nach abgeschlossener Pruefung erfolgen.

## Aktuell verbindliche Grundentscheidungen

- `ESP-NOW` ist der Funkstandard fuer v1.
- WLAN ist in v1 ausdruecklich nicht der Zielpfad.
- `Security` und `Safety` werden getrennt dokumentiert.
- Der reale Aufbau wird in dieser Ausbaustufe nur als Ablauf, Checklisten- und Nachweisstruktur dokumentiert.
- Forschungs- und Entscheidungsdokumente muessen ihre externen Quellen in einem Abschnitt `Recherchequellen` verlinken.
- Nach jedem groesseren Schritt muessen die betroffenen Dokumente aktualisiert und anschliessend `bash ./scripts/update_docs.sh` ausgefuehrt werden.

## Pflicht nach Kontextkomprimierung

Nach jeder Kontextkomprimierung oder beim Start einer neuen Sitzung ohne vollstaendigen Verlauf muessen als erstes gelesen werden:

1. `GLOBAL_RULES.md`
2. `SECURITY_FRAMEWORK.md`

Danach gelten die normalen Lesepflichten unten. Beim Wechsel in einen Unterordner ist die lokale `README.md` zu lesen, sofern der Ordnerinhalt nicht bereits aus der laufenden Sitzung bekannt ist.

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
