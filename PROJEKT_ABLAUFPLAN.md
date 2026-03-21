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

## Phase 7 - Kalibrierung und Gelenk-Mapping

### Ziel

Aus Rohdaten robuste, wiederholbare Gelenkzielwerte ableiten.

## Phase 8 - Kommunikation, Servoausfuehrung und Safety

### Ziel

Den Datenpfad vom Wearable bis zur Servoausfuehrung konsistent, ueberpruefbar und fehlertolerant aufbauen.

## Phase 9 - Integration, Leistung und Hardware-Verstetigung

### Ziel

Das Gesamtsystem unter realen Bedingungen validieren und die Hardware schrittweise in Richtung stabiler Daueraufbau fuehren.

## Querschnittspflichten fuer jede Phase

### Dokumentationspflege

- nach jedem groesseren Schritt muessen alle betroffenen Dokumente aktualisiert werden
- danach muss immer `bash ./scripts/update_docs.sh` ausgefuehrt werden
- `documentation/` ist der Verwaltungsstand und muss denselben Zwischenstand spiegeln wie die Quelldokumente

### Umgang mit lokalen Sensitivdaten

- lokale Secret-, Pairing- oder Identitaetsdaten gehoeren nur nach `security/local/`
- `.gitignore` muss solche Dateien blockieren
- diese lokalen Dateien duerfen nicht in Doku-Snapshots oder normale Projektdokumente einsickern
