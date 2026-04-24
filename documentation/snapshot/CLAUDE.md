# CLAUDE.md

Dieses Dokument dient als schneller Projektbrief fuer KI-Systeme, die ohne tieferen Verlauf in dieses Repository kommen.

## Was dieses Projekt ist

`IMU Robotic Arm` ist ein dokumentationsgetrieben vorbereitetes Embedded-Projekt fuer einen 5-DOF-Roboterarm.
Der Kern des Systems besteht aus mehreren grossen Saeulen:

- Wearable-Controller mit IMUs und Potentiometer-basierter Greifer-Eingabe
- lokaler Funkpfad ueber `ESP-NOW`
- Receiver-ESP32 als Bruecke zur Servoebene
- Arduino-basierte Servoausfuehrung
- beobachtende Debug-Bridge ueber WiFi/MQTT zum Pi
- Dashboard- und ROS-2-Digital-Twin fuer Live-Debugging
- getrennte Sicherheits-, Kalibrierungs- und Hardwaredokumentation
- nachvollziehbare Vorbereitung fuer Toolchain, Bench und spaetere Inbetriebnahme

## Was dieses Projekt noch nicht ist

- Stock-Baseline-Test des Adeept-Arms steht noch aus (Servo90, Unpacking_test_code)
- der reale Bewegungsbetrieb des Arms ist noch nicht als sicher freigegebener Produktivpfad dokumentiert
- Security-Uplift (session_id, auth_tag) noch nicht umgesetzt
- kein WLAN-, Cloud- oder Web-Interface-Projekt im sicherheitskritischen Steuerpfad

## Was bereits steht (Stand 2026-04-24)

- Toolchain: Arduino IDE 3.3.7 als Hauptumgebung, PlatformIO als lokaler Fallback und Gegencheck
- BNO055 Einzel, Dual und Triple (3x) via PCA9548A-Mux validiert (Kanaele 0/1/2)
- Greifer-Eingabe aktuell ueber Potentiometer auf `GPIO1`, weil dieser Pfad im Prototyp robuster arbeitet
- ESP-NOW Unicast als Bench-Pfad laeuft; `ImuPaket v4` mit drei IMUs, Kalibrierungsstatus, XOR-Pruefsumme und Frische-Check ist bestaetigt
- BNO055-Kalibrierungsoffsets werden im NVS persistent gespeichert und beim Boot geladen; Einzelkalibrierung per CAL0/CAL1/CAL2
- Bridge-ESP32 leitet Daten per WiFi/MQTT an Mosquitto (Pi); MQTT MCP Server erlaubt Claude Live-Sensorzugriff
- Kompletter Datenpfad validiert: Controller → ESP-NOW → Bridge → MQTT → Pi → MCP → Claude
- WiFi-Kanal 1 auf allen ESPs (Router-Kanal) fuer ESP-NOW/WiFi-Koexistenz
- Custom Board FQBN `esp32:esp32:robotic_arm_s3n16r8` — nie generisches Board verwenden (siehe `GLOBAL_RULES.md`)
- I2C-Kette Receiver → Arduino bench-validiert: ESP32 GPIO13/14 → Arduino A4/A5 (TWI Slave 0x42), Frame V1, 50Hz, alle 5 Servos mit kalibrierten Limits
- Servo-Limits empirisch kalibriert und in `calibration/servo_limits/` dokumentiert
- ISR-minimales Arduino-Design: Wire.onReceive() nur Rohbytes, Verarbeitung in loop(), Slew-Rate-Limiter (50 Grad/s)
- Controller sitzt auf Lochraster/Perfboard, der aktuelle Arm-Prototyp ist mechanisch aufgebaut
- Dashboard-Twin und ROS-2-Twin sind auf denselben Gelenkstand gemappt; Recorder, Replay, Plot und Live-Monitor sind vorhanden
- Security-Haertung mit `session_id`, Authentisierungstag und Advisory-gepruefter Stack-Basis ist dokumentiert, aber noch nicht umgesetzt

## Kein Hartcodieren — gilt fuer Firmware UND Skripte

MACs, Schluessel, Peer-Adressen und Geraete-IDs duerfen nicht direkt in `.ino`- oder `.cpp`-Dateien hartcodiert werden. Diese Werte gehoeren in gitignorierte lokale Konfigurationsdateien (`peer_config.local.h`, `credentials.local.h` o.ae.). Committed werden nur Templates oder Platzhalter-Header.

Ebenso duerfen Skripte keine absoluten lokalen Pfade enthalten — egal ob Linux, WSL oder Windows. Konfigurierbare Pfade gehoeren in gitignorierte lokale Konfigurationsdateien (z.B. `sync_config.local.sh`). Committed werden nur Templates mit generischen Platzhaltern.

## Vor jedem Push: Vollstaendige Pre-Push-Pruefung

Vor jedem `git push` sind drei Pruefungen Pflicht:

1. **Secret- und Pfad-Pruefung**: Keine Secrets, MACs, Schluessel, IPs, API-Schluessel, absolute lokale Pfade (Linux, WSL oder Windows — egal) oder Passwoerter in getrackten Dateien. Gilt ausdruecklich auch fuer Skripte, Konfigurationsdateien und Kommentare.
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

1. `MEMORY.md` — akkumulierter Projektstand, Hardware-Fakten und verbindliche Feedback-Regeln aus allen bisherigen Sessions
2. `GLOBAL_RULES.md`
3. `SECURITY_FRAMEWORK.md`

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
