# Projekt Fortschritt

Dieses Dokument fuehrt den aktuellen Managementstand des Projekts.
Es zeigt, in welcher Phase sich das Projekt befindet, welche Arbeitspakete aktiv sind und welche Punkte als Naechstes folgen.

## Gesamtstatus

- Projektmodus: bench-validierte Sensor-, Kommunikations- und Elektronikbasis mit MQTT-Bridge und Live-Dashboard-Infrastruktur
- Aktive Hauptphasen: Phase 6 - Sensorvalidierung, Phase 8 - Kommunikation und Safety, Phase 10 - Entwicklungs-Dashboard (als Bench-Werkzeug)
- Parallel gepflegt: Phase 1 - Dokumentationsfundament, Phase 2 - Security-Grundlage, Phase 3 - Vorbereitung und Toolchain, Phase 4 - Hardware-Readiness
- Entwicklungsrealitaet: Controller → Receiver + Bridge per ESP-NOW auf Kanal 1 mit ImuPaket v4 (Notaus-Flag); Bridge → Mosquitto (Pi) per WiFi/MQTT; MQTT MCP Server fuer Claude Live-Debugging; LED-Schema invertiert mit RGB GPIO48; Notaus-Toggle-Button an GPIO21; Secret-Scanner mit Git-Hooks integriert
- Naechste Schritte: Stock-Baseline-Test, UART-Grundkette, Dashboard-Views, Security-Uplift

## Aktuelle Phasenampel

| Phase | Status | Einordnung |
| --- | --- | --- |
| Phase 1 - Dokumentationsfundament | in Arbeit | Basis steht und wird nach jedem Bench-Schritt nachgezogen |
| Phase 2 - Security-Grundlage | in Arbeit | Advisory-Lage, Provisioning und Security-Baseline sind dokumentiert; Stack-Freigabe und formale Testfaelle sind offen |
| Phase 3 - Vorbereitung und Toolchain | weit fortgeschritten | Toolchain, Boardfakten und Bench-Readiness sind real bestaetigt; lokale Gegencheck- und Freigabeprozesse werden weiter geschaerft |
| Phase 4 - Hardware-Readiness und Nachweisstruktur | in Arbeit | Arm ist mechanisch aufgebaut (Stock-Zustand); Inventardoku, Stock-Baseline-Test, Batteriefach und reale Verkabelung fehlen noch |
| Phase 5 - Architektur und Portabilitaetsfundament | vorbereitet | fachlich beschrieben, aber noch nicht als echte Adapter- und Abstraktionsschicht umgesetzt |
| Phase 6 - Sensorvalidierung | weit fortgeschritten | drei BNO055, PCA9548A, Flex-Sensor und Funk-Bench sind validiert; LED-Debugging bench-validiert; Referenzbewegungen und formale Bench-Nachweise fehlen noch |
| Phase 7 - Kalibrierung und Mapping | vorbereitet | startet nach Arm-Aufbau, Referenzpose und erster Servo-Grundfunktion |
| Phase 8 - Kommunikation, Servoausfuehrung und Safety | in Arbeit | `ESP-NOW`-Benchpfad ist validiert; Security-Uplift, UART und Arduino-Safety-Logik sind offen |
| Phase 9 - Integration und Hardware-Verstetigung | nicht gestartet | folgt nach erster lauffaehiger Gesamtkette bis zur Servoebene |
| Phase 10 - Zukunftsausbau und Systemerweiterung | vorbereitet | als Post-v1-Ausblick dokumentiert, aber nicht aktiv |

## Bereits erledigt

- [x] Root-Dokumente fuer Architektur, Regeln, Kommunikation, Kalibrierung und Safety angelegt
- [x] `PROJEKT_ABLAUFPLAN.md` und `PROJEKT_FORTSCHRITT.md` als Managementschicht eingefuehrt
- [x] `SECURITY_FRAMEWORK.md` und `security/` als eigener Bereich angelegt
- [x] advisory-basierte `ESP-NOW`-Schwachstellen, Grenzen und fruehe Gegenmassnahmen in Security- und Kommunikationsdokumenten nachgezogen
- [x] `preparation/` als eigener Vorbereitungsbereich angelegt
- [x] `hardware/assembly/` und `hardware/bringup/` als neue Hardware-Unterbereiche eingefuehrt
- [x] `hardware/electronics/` fuer ESP32-S3-Integration, Pinmapping, Debugsignale und Breadboard-/Lochrasterpfad eingefuehrt
- [x] `future/` fuer spaetere Plattform-, Teleoperations- und Portabilitaetsausbaustufen eingefuehrt
- [x] `docs/templates/` mit Roboterarm-bezogenen Vorlagen eingefuehrt
- [x] Wearable-Konzept fuer IMU-Positionierung, Prototyp und spaeteren Betriebstyp dokumentiert
- [x] `.gitignore` und `security/local/` fuer lokale, nicht-repotaugliche Daten vorbereitet
- [x] `CLAUDE.md` als kompakter KI-Brief eingefuehrt
- [x] Quellenpflicht fuer forschungs- und entscheidungsgetriebene Dokumente in Regeln und Hardware-Doku verankert
- [x] offizieller Adeept-V4.0-Download mit Tutorial-PDFs, Originalcode und Schaltplan unter `official_downloads/` uebernommen und projektrelevant ausgewertet
- [x] Toolchain vollstaendig eingerichtet: Arduino IDE 3.3.7, ESP32 Core 3.3.7, boards.local.txt fuer N16R8 und PlatformIO als Gegencheck dokumentiert und einsatzbereit
- [x] echte Boardrevision der vorhandenen ESP32-S3-DevKits und die RGB-LED-Pins am Hardwarebestand geprueft — v1.0, RGB auf GPIO48, GPIO35/36/37 intern belegt
- [x] ESP-NOW Hello-World erfolgreich zwischen Controller und Receiver getestet
- [x] Beide Board-MACs dokumentiert in `security/local/device_identities.local.txt`
- [x] ESP-NOW Unicast erfolgreich getestet (Controller -> Receiver per MAC, bestaetigt 2026-03-22)
- [x] BNO055 I2C-Scan erfolgreich: Adresse 0x29 (ADR=3V3), SDA GPIO8 / SCL GPIO9
- [x] BNO055 Rohwerte erfolgreich ausgelesen: Euler-Winkel stabil, Gyro-Kalibrierung 3/3
- [x] PCA9548A Mux erfolgreich: BNO055 ueber Kanal 0 angesprochen, Sys/Gyro/Accel 3/3, Mux-Adresse 0x70
- [x] Zwei BNO055 gleichzeitig ueber Mux-Kanaele 0 und 1 ausgelesen, beide Sensoren stabil
- [x] IMU-Daten per ESP-NOW Unicast uebertragen: `ImuPaket v1` mit XOR-Pruefsumme und Frische-Check, alle Bench-Pakete valide
- [x] Flex-Sensor Rohmessung: gerade=1108, maximal gebogen=940, Bereich 168 Counts, GPIO1
- [x] No-Hardcoding-Regel eingefuehrt: MACs in `peer_config.local.h` ausgelagert, gitignoriert
- [x] Akku- und Ladegeraet-Auswahl als Projektarbeitsstand festgelegt: `4x Molicel INR-18650-M35A` ohne Loetfahne plus `1x XTAR VC4SL`
- [x] Projektentscheidung zu den wichtigsten Stock-Funktionen festgezogen: OLED uebernehmen, Potentiometer-Steuerung als Fallback behalten, Learning/Action Memory uebernehmen, Processing vorerst als Referenz- und Fallbackpfad behalten
- [x] minimaler UART-Frame v1 fuer `Receiver -> Arduino` dokumentiert: feste Startbytes, Version, Sequenz, Flags, 5 Achs-Sollwerte, CRC8
- [x] dritten BNO055 ueber PCA9548A Mux-Kanal 2 angeschlossen und bench-validiert: alle drei Sensoren liefern live Euler-Daten ueber ESP-NOW, Pakete valide (bestaetigt 2026-03-26)
- [x] ImuPaket auf Protokollversion 3 angehoben: KalibStatus (sys/gyro/accel/mag) pro Sensor im Paket, Mux-Delay auf 10ms erhoeht (bestaetigt 2026-03-26)
- [x] BNO055-Kalibrierungspersistenz im NVS eingebaut: Offsets werden automatisch gespeichert wenn Gyro>=3, Accel>=2, Mag>=2 und beim Boot wiederhergestellt (bestaetigt 2026-03-26)
- [x] Einzelkalibrierungsmodus eingefuehrt: CAL0/CAL1/CAL2 per Serial fuer fokussierte Sensor-Kalibrierung, RECAL zum Zuruecksetzen (bestaetigt 2026-03-26)
- [x] Firmware-Versionsarchiv angelegt: espnow_imu_v1, espnow_receiver_v1, espnow_imu_v2, espnow_receiver_v2 als Bench-Snapshots unter firmware/ (bestaetigt 2026-03-26)
- [x] Adeept 5-DOF Roboterarm mechanisch im Stock-Zustand aufgebaut — Fotos unter docs/photos/ (bestaetigt 2026-03-24)
- [x] LED-Debugging invertiert (aus=OK, an=Problem) mit RGB GPIO48 als FAULT/NOTAUS auf allen ESPs (bestaetigt 2026-03-26, aktualisiert 2026-03-28)
- [x] Live-Sensorausfallerkennung und Flex-Sensor-Plausibilitaetspruefung im Controller (bestaetigt 2026-03-26)
- [x] Bridge-ESP32 Firmware: ESP-NOW Empfang, MQTT-Weiterleitung (PubSubClient), OTA (ArduinoOTA), NeoPixel RGB (bestaetigt 2026-03-26)
- [x] Controller Multi-Peer: sendet ImuPaket v4 an Receiver (Steuerpfad) und Bridge (Debug-Pfad) gleichzeitig (bestaetigt 2026-03-26)
- [x] WiFi-Kanal 1 auf allen ESPs fuer ESP-NOW/WiFi-Koexistenz mit Router (bestaetigt 2026-03-26)
- [x] Mosquitto MQTT-Broker auf Pi konfiguriert: Passwort-Auth, WebSocket-Listener auf 9001, Nginx Reverse Proxy auf /mqtt/ (bestaetigt 2026-03-26)
- [x] MQTT MCP Server (dashboard/mcp/mqtt_mcp_server.py) mit 6 Tools fuer Claude Live-Sensorzugriff (bestaetigt 2026-03-26)
- [x] Secret-Scanner (scripts/secret_scan.sh) mit 10 Kategorien, Pre-Commit/Pre-Push Hooks und GitHub Actions Workflow (bestaetigt 2026-03-26)
- [x] Bidirektionale Sync-Skripte Windows/WSL ohne absolute Pfade (bestaetigt 2026-03-26)
- [x] Live-IMU-Daten ueber kompletten Pfad validiert: Controller → ESP-NOW → Bridge → WiFi/MQTT → Pi → MCP → Claude (bestaetigt 2026-03-26)
- [x] ImuPaket auf Protokollversion 4 angehoben: Notaus-Flag (Bit 0) im neuen `flags`-Feld, Toggle-Button an GPIO21, alle drei Firmwares aktualisiert (bestaetigt 2026-03-28)
- [x] Notaus-Toggle-Button auf GPIO21 am Controller: jeder Tastendruck toggelt Notaus-Zustand, interner Pull-Up, 50ms Entprellung, kein externer Pull-Up noetig (bestaetigt 2026-03-28)
- [x] RGB-LED GPIO48 auf allen drei ESPs: orange blinkend bei Notaus (hoechste Prio), rot blinkend bei Fehler, aus wenn OK (bestaetigt 2026-03-28)
- [x] LED-Schema am Controller von blinken auf an/aus umgestellt: aus = OK, an = Problem (bestaetigt 2026-03-28)
- [x] Notaus-Kette end-to-end validiert: Controller (Toggle-Button GPIO21) → ImuPaket v4 flags → Receiver (RGB orange) + Bridge (RGB orange, MQTT `"notaus":true/false`) (bestaetigt 2026-03-28)
- [x] Alle Dokumentation auf GPIO21, Toggle-Button, Kanal 1 und LED-Schema an=Problem durchgezogen (bestaetigt 2026-03-28)
- [x] Foto des aktuellen Bench-Aufbaus unter docs/photos/2026-03-28_bench_aufbau_aktuell.jpg abgelegt (2026-03-28)

## Noch offen im aktuellen Schwerpunkt

- [ ] Root-Dokumente, lokale Bereichsdokumente und neue Security-/Preparation-Bereiche dauerhaft synchron halten
- [ ] konkrete `ESP-NOW`-/`ESP-IDF`-Zielbasis fuer Realbetrieb freigeben
- [ ] einfache UART-Grundkette `Receiver -> Arduino` bench-validieren
- [ ] Bench-Protokoll auf die dokumentierte Security-Baseline mit `session_id` und Authentisierungstag anheben
- [ ] Stock-Baseline-Test des aufgebauten Adeept-Arms durchfuehren und dokumentieren
- [ ] ausgewaehlte Akkus und das Ladegeraet beschaffen und pruefen
- [ ] Dashboard-Views im bestehenden IoT Control Center auf dem Pi implementieren
- [ ] PPS-Optimierung bei Multi-Peer ESP-NOW (aktuell ~1-2 PPS statt 20)
- [ ] OTA Dual-Partition Rollback bench-validieren
- [ ] Security-, Safety- und Kommunikationsfaelle in konkrete Test- und Messdokumente herunterbrechen

## Naechste sinnvolle Arbeitspakete

1. Stock-Baseline-Test des aufgebauten Adeept-Arms durchfuehren (Servo90, Unpacking_test_code)
2. Dashboard-Views im bestehenden IoT Control Center auf dem Pi implementieren (Debug-Konsole, Statistiken, 3D-Simulation)
3. UART-Pfad Receiver → Arduino in Betrieb nehmen
4. Security-Uplift erst danach auf die Grundkette aufsetzen

## Leitende Dokumente fuer den aktuellen Stand

- `README.md` als Einstieg
- `CLAUDE.md` als Kurzbrief
- `ROADMAP.md` als globale Phasenplanung
- `PROJEKT_ABLAUFPLAN.md` als Management-Reihenfolge
- `SECURITY_FRAMEWORK.md`, `COMMUNICATION_FRAMEWORK.md`, `SAFETY_FRAMEWORK.md` und `CALIBRATION_FRAMEWORK.md` als Systemrahmen
- `preparation/README.md`, `security/README.md`, `hardware/README.md`, `docs/templates/README.md` und `future/README.md` als Schwerpunktbereiche

## Offene Management-Risiken

- Ohne konsequente Synchronisierung koennen Root-Dokumente, lokale Readmes und Roadmaps erneut auseinanderlaufen.
- Die aktuelle `ESP-NOW`-Bench-Variante ist fuer Sensor- und Transporttests geeignet, aber noch kein freigegebener Security-Produktivpfad.
- Roboterarm-Aufbau, UART-Kette, Servo-Limits und reale Safety-Fallbacks sind noch nicht bench-validiert und bleiben damit der groesste Integrationsblocker.
- Toolchain- und Security-Freigaben muessen sauber zwischen committed Zielbasis und lokal geflashtem Ist-Stand getrennt dokumentiert bleiben.

## Pflegepflicht fuer dieses Dokument

- nach jedem groesseren Meilenstein aktualisieren
- bei Phasenwechseln den Status dieser Datei und der `ROADMAP.md` gemeinsam anpassen
- nach jeder relevanten Aenderung `bash ./scripts/update_docs.sh` ausfuehren
