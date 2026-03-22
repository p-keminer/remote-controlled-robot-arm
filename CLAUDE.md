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
- noch keine vollstaendige Firmwarekette bis zum Servo
- kein WLAN-, Cloud- oder Web-Interface-Projekt

## Was bereits steht (Stand 2026-03-22)

- Toolchain: Arduino IDE 3.3.7 + PlatformIO
- BNO055 Einzel + Dual via PCA9548A-Mux validiert
- Flex-Sensor ADC-Pfad ausgelesen und kalibriert
- ESP-NOW Unicast mit ImuPaket v1 laeuft (Pruefsumme, Frische-Check)

## Kein Hartcodieren in Firmware

MACs, Schluessel, Peer-Adressen und Geraete-IDs duerfen nicht direkt in `.ino`- oder `.cpp`-Dateien hartcodiert werden. Diese Werte gehoeren in gitignorierte lokale Konfigurationsdateien (`peer_config.local.h`, `credentials.local.h` o.ae.). Committed werden nur Templates oder Platzhalter-Header.

## Vor jedem Push: Vollstaendige Pre-Push-Pruefung

Vor jedem `git push` sind drei Pruefungen Pflicht:

1. **Secret-Pruefung**: Keine Secrets, MACs, Schluessel, IP-Adressen oder Passwoerter in getrackten Dateien, die nicht durch `.gitignore` geblockt sind.
2. **Dokumenten-Konsistenz**: Alle abgehakten Punkte `[x]` muessen ueber `ROADMAP.md`, `PROJEKT_FORTSCHRITT.md`, `PROJEKT_ABLAUFPLAN.md` und lokale Roadmaps konsistent und widerspruchsfrei sein.
3. **README-Aktualitaet**: Die Abschnitte `Aktueller Fokus` und `Aktueller Entwicklungsstand` in `README.md` muessen den echten Projektstand widerspiegeln.

Ein Push darf erst erfolgen, wenn alle drei Pruefungen abgeschlossen sind.

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
