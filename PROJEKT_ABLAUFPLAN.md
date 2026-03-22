# Projekt Ablaufplan

Dieses Dokument beschreibt den projektmanagementorientierten roten Faden fuer das gesamte Roboterarm-Projekt.
Es verknuepft Phasen, Arbeitspakete, Leitdokumente und Pflegepflichten in einer festen Reihenfolge.

## Ziel des Ablaufplans

- einen klaren Startpunkt fuer das Projekt festlegen
- Security, Vorbereitung und Hardwarefokus sichtbar vor die eigentliche Firmwarearbeit setzen
- sicherstellen, dass Dokumentation, Architektur, Aufbau, Tests und spaetere Implementierung konsistent wachsen
- verhindern, dass Hardwarewissen, Security-Annahmen oder lokale Secret-Daten ungeordnet im Projekt verstreut werden

## Projektstartpunkt

Der aktuelle und verbindliche Startpunkt ist:

### Phase 1 - Dokumente ausarbeiten und Projektfuehrung festziehen

Das erste aktive Arbeitspaket bleibt die tiefe Ausarbeitung der Dokumentation.
Vor groesseren Firmware- oder Hardwarearbeiten muessen Root-Dokumente, Security-Regeln, Vorbereitungsbereiche, Hardware-Readiness und Vorlagenbasis stehen.

## Phasenmodell

## Phase 1 - Dokumentationsfundament und Regelwerk

### Ziel

Eine durchgaengige Projektfuehrung aufbauen, damit alle spaeteren Entwicklungsphasen auf denselben Regeln, Begriffen und Dokumentpfaden beruhen.

### Leitdokumente

- `README.md`
- `CLAUDE.md`
- `ROADMAP.md`
- `PROJEKT_ABLAUFPLAN.md`
- `PROJEKT_FORTSCHRITT.md`
- `GLOBAL_RULES.md`
- `AGENTS.md`

## Phase 2 - Security-Grundlage

### Ziel

Das Projekt fuer lokalen, sicheren `ESP-NOW`- und `UART`-Betrieb dokumentieren, ohne WLAN oder Cloud als v1-Ziel einzufuehren.

### Typische Arbeitspakete

- Bedrohungsmodell und Trust Boundaries beschreiben
- Kommunikations- und Schnittstellen-Security dokumentieren
- lokale Secret- und Identitaetsdateien sauber aus Git heraushalten
- Security-Testbasis und Abwehrfaelle vorbereiten

### Leitdokumente

- `SECURITY_FRAMEWORK.md`
- `security/README.md`
- `security/THREAT_MODEL.md`
- `security/COMMUNICATION_SECURITY.md`
- `security/PROVISIONING_AND_DEBUG_RULES.md`

## Phase 3 - Vorbereitung und Toolchain

### Ziel

Vorbedingungen fuer Arduino IDE, ESP32-Umgebung, Bench und spaetere Inbetriebnahme dokumentieren, bevor echte Implementierung oder Aufbau startet.

### Typische Arbeitspakete

- Toolchainpfad fuer Arduino IDE beschreiben
- ESP32-Voraussetzungen und Bibliotheksbasis strukturieren
- Bench- und Readiness-Checklisten anlegen
- Vorbedingungen fuer Aufbau und erste Stromtests festschreiben

### Leitdokumente

- `preparation/README.md`
- `preparation/arduino_ide_setup/README.md`
- `preparation/esp32_environment/README.md`
- `preparation/bench_setup/README.md`
- `preparation/robot_arm_readiness/README.md`

## Phase 4 - Hardware-Readiness und Nachweisstruktur

### Ziel

Die reale Hardwarebetrachtung von Anfang an sichtbar machen, ohne den Arm in dieser Ausbaustufe bereits aufzubauen.

### Typische Arbeitspakete

- Inventar- und Materialstand dokumentierbar machen
- Aufbauablauf und Bringup-Reihenfolge beschreiben
- Templates fuer Inventar, Aufbau, Verkabelung, Sensormontage und Erstinbetriebnahme bereitstellen
- Hardwareabweichungen spaeter sauber gegen Dokumente pruefen koennen

### Leitdokumente

- `hardware/README.md`
- `hardware/assembly/README.md`
- `hardware/bringup/README.md`
- `docs/templates/README.md`

## Phase 5 - Architektur und Portabilitaetsfundament

### Ziel

Die technische Grundlage so beschreiben und spaeter implementieren, dass das Projekt erweiterbar bleibt und nicht an genau eine Sensor-, Controller- oder Funkkombination gekettet ist.

## Phase 6 - Sensorik-Basis und Rohdatenvalidierung

### Ziel

Die Sensorseite des Systems technisch sicher in Betrieb nehmen und Rohdaten nachvollziehbar verifizieren.

### Typische Arbeitspakete

- BNO055 Einzelsensor und Mux-Pfad validieren (bestätigt 2026-03-22)
- Flex-Sensor ADC-Pfad validieren und Rohwerte dokumentieren (bestätigt 2026-03-22)
- IMU-Daten per ESP-NOW uebertragen, Paketformat und Integritaet pruefen (bestätigt 2026-03-22)
- LED-Debugging einbauen: Sender GPIO4/5/6 (IMU-Status), GPIO7 (COMMS), GPIO10 (FAULT)
- Buzzer-Pfad GPIO21 mit sicherem Default-Off pruefen
- Receiver-LEDs GPIO4/5/6 einbauen
- Flex-Sensor nach Montage neu kalibrieren

### Leitdokumente

- `hardware/electronics/PIN_MAPPING_CONFIRMED.md`
- `hardware/electronics/SCHEMATIC_CURRENT.md`
- `hardware/electronics/DEBUG_SIGNALS_AND_BUZZER.md`
- `calibration/flex_sensor/MESSWERTE.md`

## Phase 7 - Kalibrierung und Gelenk-Mapping

### Ziel

Aus Rohdaten robuste, wiederholbare Gelenkzielwerte ableiten.

### Typische Arbeitspakete

- Roboterarm aufbauen und Servo-Grundfunktion pruefen
- Gelenklimits pro Achse definieren und dokumentieren
- IMU-Winkel auf Servo-Sollwerte mappen
- Flex-Sensor auf Greifer-Oeffnungsgrad mappen
- Kalibrierungsdaten in `calibration/` dokumentieren

## Phase 8 - Kommunikation, Servoausfuehrung und Safety

### Ziel

Den Datenpfad vom Wearable bis zur Servoausfuehrung konsistent, ueberpruefbar und fehlertolerant aufbauen.

### Typische Arbeitspakete

- UART-Pfad Receiver → Arduino in Betrieb nehmen (GPIO15/GPIO16)
- Arduino-Firmware fuer Servo-Ausfuehrung mit Limits und Rampen schreiben
- Watchdog, Timeout und Neutralverhalten implementieren
- Safety-Fallback und Buzzer-Warnlogik einbauen
- Gesamtkette Wearable → ESP-NOW → Receiver → UART → Arduino → Servo testen

## Phase 9 - Integration, Leistung und Hardware-Verstetigung

### Ziel

Das Gesamtsystem unter realen Bedingungen validieren und die Hardware schrittweise in Richtung stabiler Daueraufbau fuehren.

## Phase 10 - Zukunftsausbau und Systemerweiterung

### Ziel

Nach dem Grundsystem zusaetzliche Plattform-, Teleoperations- und Portabilitaetslinien geordnet vorbereiten, ohne die v1-Basis mit Zukunftsideen zu vermischen.

### Typische Arbeitspakete

- mobilen Unterbau fuer das Gesamtsystem konzipieren
- Dashboard- und Controller-Ebene fuer Fahrzeug und Arm planen
- Kamerasicht und Teleoperationspfad vorbereiten
- Abstraktionsschicht zwischen Bewegungslogik und konkreter Servo-/Aktuatorausgabe definieren

### Leitdokumente

- `future/README.md`
- `future/FUTURE_WORK.md`
- `future/ROADMAP.md`

## Querschnittspflichten fuer jede Phase

### Dokumentationspflege

- nach jedem groesseren Schritt muessen alle betroffenen Dokumente aktualisiert werden
- danach muss immer `bash ./scripts/update_docs.sh` ausgefuehrt werden
- `documentation/` ist der Verwaltungsstand und muss denselben Zwischenstand spiegeln wie die Quelldokumente

### Quellenpflege

- forschungs- und entscheidungsgetriebene Dokumente fuehren ihre externen Quellen in einem Abschnitt `Recherchequellen`
- neue technische Empfehlungen, Layoutideen oder Sicherheitsannahmen muessen dort mit Link hinterlegt werden
- wenn Entscheidungen nur auf internen Tests, Fotos oder Messungen beruhen, ist das im Dokument klar als interne Grundlage kenntlich zu machen

### Umgang mit lokalen Sensitivdaten

- lokale Secret-, Pairing- oder Identitaetsdaten gehoeren nur nach `security/local/`
- `.gitignore` muss solche Dateien blockieren
- diese lokalen Dateien duerfen nicht in Doku-Snapshots oder normale Projektdokumente einsickern
