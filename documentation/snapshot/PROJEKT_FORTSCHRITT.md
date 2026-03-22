# Projekt Fortschritt

Dieses Dokument fuehrt den aktuellen Managementstand des Projekts.
Es zeigt, in welcher Phase sich das Projekt befindet, welche Arbeitspakete aktiv sind und welche Punkte als Naechstes folgen.

## Gesamtstatus

- Projektmodus: Dokumentations-, Security-, Vorbereitungs- und Hardware-Readiness-Aufbau
- Aktive Hauptphase: Phase 1 - Dokumentationsfundament
- Parallel ausgebaut: Phase 2 - Security-Grundlage, Phase 3 - Vorbereitung, Phase 4 - Hardware-Readiness
- Entwicklungsrealitaet: Toolchain steht (Arduino IDE 3.3.7 + PlatformIO), ESP-NOW Hello-World zwischen beiden Boards erfolgreich getestet (2026-03-22)

## Aktuelle Phasenampel

| Phase | Status | Einordnung |
| --- | --- | --- |
| Phase 1 - Dokumentationsfundament | in Arbeit | Basis steht und wird weiter synchronisiert |
| Phase 2 - Security-Grundlage | in Arbeit | eigener Bereich ist angelegt |
| Phase 3 - Vorbereitung und Toolchain | in Arbeit | Dokumentationsstruktur steht, reale Setups fehlen |
| Phase 4 - Hardware-Readiness und Nachweisstruktur | in Arbeit | Vorlagen und Hardwareablauf sind im Aufbau |
| Phase 5 - Architektur und Portabilitaetsfundament | vorbereitet | fachlich beschrieben, aber noch nicht technisch heruntergebrochen |
| Phase 6 - Sensorvalidierung | vorbereitet | wartet auf Toolchain, Hardware und Bench-Bereitschaft |
| Phase 7 - Kalibrierung und Mapping | vorbereitet | haengt von realen Sensordaten ab |
| Phase 8 - Kommunikation, Servoausfuehrung und Safety | vorbereitet | Kommunikations- und Safety-Basis ist dokumentiert |
| Phase 9 - Integration und Hardware-Verstetigung | nicht gestartet | folgt erst nach ersten lauffaehigen Teilketten |
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

## Noch offen im aktuellen Schwerpunkt

- [ ] Root-Dokumente, lokale Bereichsdokumente und neue Security-/Preparation-Bereiche vollstaendig gegeneinander synchron halten
- [ ] Quellenkonvention kuenftig in allen neuen Fach- und Entscheidungsdokumenten konsequent mitfuehren
- [x] reale Installations- und Readiness-Schritte fuer Arduino IDE, ESP32-Umgebung und Bench mit echtem Stand fuellen — **Arduino IDE 3.3.7, ESP32 Core 3.3.7, boards.local.txt, PlatformIO (bestätigt 2026-03-22)**
- [x] echte Boardrevision der vorhandenen ESP32-S3-DevKits und die RGB-LED-Pins am Hardwarebestand pruefen — **v1.0, RGB auf GPIO48, GPIO35/36/37 intern belegt (bestätigt 2026-03-22)**
- [ ] bevorzugten Pinplan fuer Sender und Receiver gegen echte BNO055-, Mux-, ADC- und UART-Tests absichern
- [ ] konkrete `ESP-NOW`- und `ESP-IDF`-Version des spaeteren Stacks gegen bekannte Advisories freigeben
- [ ] Bench-Protokoll von XOR-/Frische-Check auf die dokumentierte Security-Baseline mit `session_id` und Authentisierungstag anheben
- [ ] reale Hardware-, Inventar- und Aufbauinformationen spaeter in die vorbereiteten Vorlagen ueberfuehren
- [ ] Security-, Safety- und Kommunikationsfaelle spaeter in konkrete Test- und Messdokumente herunterbrechen

## Bereits erledigt (Ergaenzung 2026-03-22)

- [x] Toolchain vollstaendig eingerichtet: Arduino IDE 3.3.7 + boards.local.txt fuer N16R8, PlatformIO in WSL als Fallback
- [x] ESP-NOW Hello-World erfolgreich zwischen Controller und Receiver getestet
- [x] Beide Board-MACs dokumentiert in `security/local/device_identities.local.txt`
- [x] ESP-NOW Unicast erfolgreich getestet (Controller -> Receiver per MAC, bestätigt 2026-03-22)
- [x] BNO055 I2C-Scan erfolgreich: Adresse 0x29 (ADR=3V3), SDA GPIO8 / SCL GPIO9 (bestätigt 2026-03-22)
- [x] BNO055 Rohwerte erfolgreich ausgelesen: Euler-Winkel stabil, Gyro-Kalibrierung 3/3 (bestätigt 2026-03-22)
- [x] PCA9548A Mux erfolgreich: BNO055 über Kanal 0 angesprochen, Sys/Gyro/Accel 3/3, Mux-Adresse 0x70 (bestätigt 2026-03-22)
- [x] Zwei BNO055 gleichzeitig über Mux-Kanäle 0 und 1 ausgelesen, beide Sensoren stabil (bestätigt 2026-03-22)
- [x] IMU-Daten per ESP-NOW Unicast übertragen: ImuPaket v1 mit Prüfsumme und Frische-Check, alle Pakete valide (bestätigt 2026-03-22)
- [x] Flex-Sensor Rohmessung: gerade=1108, maximal gebogen=940, Bereich 168 Counts, GPIO1 (bestätigt 2026-03-22)
- [x] No-Hardcoding-Regel eingeführt: MACs in `peer_config.local.h` ausgelagert, gitignoriert

## Naechste sinnvolle Arbeitspakete

1. LED-Debugging einbauen: Sender GPIO4/5/6 (IMU-Status), GPIO7 (COMMS), GPIO10 (FAULT) — siehe `hardware/electronics/DEBUG_SIGNALS_AND_BUZZER.md`
2. Buzzer-Pfad GPIO21 mit sicherem Default-Off pruefen
3. Receiver-LEDs GPIO4/5/6 einbauen
4. Dann: Roboterarm aufbauen und UART-Pfad Receiver → Arduino in Betrieb nehmen

## Leitende Dokumente fuer den aktuellen Stand

- `README.md` als Einstieg
- `CLAUDE.md` als Kurzbrief
- `ROADMAP.md` als globale Phasenplanung
- `PROJEKT_ABLAUFPLAN.md` als Management-Reihenfolge
- `SECURITY_FRAMEWORK.md`, `COMMUNICATION_FRAMEWORK.md`, `SAFETY_FRAMEWORK.md` und `CALIBRATION_FRAMEWORK.md` als Systemrahmen
- `preparation/README.md`, `security/README.md`, `hardware/README.md`, `docs/templates/README.md` und `future/README.md` als neue Schwerpunktbereiche

## Offene Management-Risiken

- Die Dokumentbasis ist jetzt breiter, aber reale Toolchain- und Hardware-Nachweise fehlen noch.
- Ohne konsequente Nutzung der neuen Vorlagen droht der Hardwareteil wieder hinter dem Softwareteil zurueckzufallen.
- Lokale Secret- oder Pairingdaten muessen strikt aus der normalen Dokumentation herausgehalten werden.

## Pflegepflicht fuer dieses Dokument

- nach jedem groesseren Meilenstein aktualisieren
- bei Phasenwechseln den Status dieser Datei und der `ROADMAP.md` gemeinsam anpassen
- nach jeder relevanten Aenderung `bash ./scripts/update_docs.sh` ausfuehren
