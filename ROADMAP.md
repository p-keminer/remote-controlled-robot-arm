# Globale Roadmap

Dieses Dokument beschreibt den uebergeordneten Ablaufplan fuer das gesamte Roboterarm-Projekt.

## Phase 1 - Dokumentationsfundament

### Arbeitspaket 1.1 - Projektstruktur

- [x] Root-Dokumente fuer Regeln, Architektur, Frameworks und Sicherheit anlegen
- [x] relevante Arbeitsordner mit `README.md` strukturieren
- [x] lokale `ROADMAP.md`-Dateien fuer groessere Teilprojekte anlegen
- [x] generierten Dokumentationsordner und Snapshot-Logik vorbereiten

### Arbeitspaket 1.2 - Arbeitsablaeufe

- [x] `docs/` als manuelle Projektarbeitsdokumentation festlegen
- [x] `documentation/` als generierten Verwaltungsordner festlegen
- [x] WSL-faehiges Skript fuer Dokumentationsupdates anlegen
- [ ] lokale und globale Roadmaps im Alltag synchron halten

### Arbeitspaket 1.3 - Projektmanagement und Regelpraezisierung

- [x] `PROJEKT_ABLAUFPLAN.md` als roter Faden fuer alle Projektphasen anlegen
- [x] `PROJEKT_FORTSCHRITT.md` fuer den laufenden Managementstand anlegen
- [x] Regeln fuer Modularitaet, Abstraktion, Skalierbarkeit und Hardware-Portabilitaet nachschaerfen
- [x] Pflegepflicht fuer Doku-Updates nach groesseren Schritten explizit festziehen
- [x] Quellenpflicht fuer Forschungs- und Entscheidungsdokumente projektweit festschreiben

## Phase 2 - Security-Grundlage

### Arbeitspaket 2.1 - Sicherheitsrahmen

- [x] `SECURITY_FRAMEWORK.md` als Root-Dokument einfuehren
- [x] `security/` mit Bedrohungsmodell, Kommunikations- und Provisioning-Regeln anlegen
- [x] `ESP-NOW` als lokalen v1-Funkpfad ohne WLAN dokumentieren
- [x] lokale Secret- und Geraetedaten per `.gitignore` und `security/local/` absichern

### Arbeitspaket 2.2 - Security-Nachweisbasis

- [x] `tests/security/` als eigenen Nachweisbereich einfuehren
- [x] advisory-basierte `ESP-NOW`-Schwachstellen, bekannte Grenzen und erste Gegenmassnahmen dokumentieren
- [ ] Security-Szenarien spaeter in konkrete Testfaelle herunterbrechen
- [ ] konkrete `ESP-NOW`- bzw. `ESP-IDF`-Stack-Version vor Realbetrieb gegen bekannte Advisories freigeben

## Phase 3 - Vorbereitung und Toolchain

### Arbeitspaket 3.1 - Entwicklungsbasis

- [x] `preparation/` als eigenen Bereich fuer Vorbedingungen und Setup anlegen
- [x] Arduino IDE als Hauptumgebung und PlatformIO als lokalen Fallback dokumentieren
- [x] ESP32-Umgebung, Bench und Readiness als Checklistenbereiche strukturieren
- [x] reale Installations- und Verifikationsschritte nachgezogen — Arduino IDE 3.3.7, ESP32 Core 3.3.7, boards.local.txt fuer N16R8 und PlatformIO als Gegencheck dokumentiert und bench-validiert (bestaetigt 2026-03-22)

### Arbeitspaket 3.2 - Vorbereitungsfreigabe

- [x] Toolchain-Bereitschaft gegen Dokumentation geprueft — Arduino IDE 3.3.7 als Hauptumgebung und PlatformIO als Fallback einsatzbereit (bestaetigt 2026-03-22)
- [ ] Bench-Vorbedingungen und Sicherheitsfreigaben konkretisieren
- [x] Readiness vor erster Hardware- oder Firmwarearbeit dokumentiert und erreicht (bestaetigt 2026-03-22)

## Phase 4 - Hardware-Readiness und Dokumentationsvorlagen

### Arbeitspaket 4.1 - Hardwaredokumentation

- [x] `hardware/assembly/` und `hardware/bringup/` als neue Unterbereiche einfuehren
- [x] Inventar-, Aufbau-, Verkabelungs- und Einschaltvorlagen unter `docs/templates/` anlegen
- [x] Elektronikbereich fuer ESP-Integration, Breadboardpfad, Lochrasterpfad, Pinmapping und Debugsignale dokumentieren
- [x] konkrete Produktbasis des vorhandenen Adeept-Arms mit offizieller Package- und Stromversorgungsbasis dokumentieren
- [x] offiziellen Adeept-V4.0-Download mit Tutorial-PDFs, Originalcode und Schaltplan ins Repo uebernehmen und auswerten
- [ ] reale Stueckliste, Verkabelung und Aufbauabweichungen spaeter nachziehen

### Arbeitspaket 4.2 - Nachweisstruktur

- [ ] Hardware-Inventar gegen realen Materialstand dokumentieren
- [ ] offiziellen Stock-Baseline-Test des originalen Adeept-Arms vor Umbauten dokumentieren
- [x] Adeept 5-DOF Roboterarm mechanisch im Stock-Zustand aufgebaut — Fotos unter docs/photos/ (bestaetigt 2026-03-24)
- [ ] Aufbauablauf und Bringup-Reihenfolge mit echten Nachweisen fuellen
- [ ] Sensormontage und erste Stromtests sauber protokollieren
- [ ] Arbeitsstand `5 aktive Servos + 1 Reserve/Testservo` am realen Kit bestaetigen und gegen echten Aufbau dokumentieren
- [x] Akku- und Ladegeraet-Auswahl fuer den ersten Projektstand dokumentieren — `4x Molicel INR-18650-M35A` ohne Loetfahne plus `1x XTAR VC4SL` als Beschaffungsbasis festgelegt
- [ ] reale Beschaffung, Batteriefach-Passung und Strompfad fuer reale Servo-Bewegung gegen den aufgebauten Arm bestaetigen
- [x] Boardrevision, Pinmapping und Debugkonzept auf echter Bench-Hardware validiert — v1.0, RGB auf GPIO48, GPIO35/36/37 intern belegt, GPIO8/9 I2C bestaetigt, GPIO1 ADC1 bestaetigt (bestaetigt 2026-03-22)

## Phase 5 - Architektur und Portabilitaetsfundament

### Arbeitspaket 5.1 - Schnittstellenklarheit

- [ ] Hardware-Abstraktionspunkte fuer Sensorik, Transport und Servoausgabe festziehen
- [ ] Konfigurations- und Kalibrierdaten sauber von harter Laufzeitlogik trennen
- [ ] Erweiterbarkeit auf andere Hardwarepfade fachlich vorbereiten

## Phase 6 - Sensorvalidierung

### Arbeitspaket 6.1 - IMU-Basis

- [x] einzelne BNO055 am Controller lesen — I2C-Scan, Adresse 0x29 (ADR=3V3), SDA GPIO8 / SCL GPIO9, Euler-Winkel stabil, Gyro-Kalibrierung 3/3 (bestaetigt 2026-03-22)
- [x] PCA9548A-Kanalumschaltung fuer mehrere IMUs testen — Mux 0x70, Kanal 0 und 1 einzeln und sequenziell umschalten (bestaetigt 2026-03-22)
- [x] gleichzeitiges Auslesen aller benoetigten IMUs validieren — zwei BNO055 ueber Kanal 0 und 1, beide stabil, Sys/Gyro/Accel 3/3 (bestaetigt 2026-03-22)
- [x] dritten BNO055 ueber Mux-Kanal 2 anbinden und als dritte Segmentquelle bench-validieren — alle drei Sensoren liefern live Euler-Daten ueber ESP-NOW, Kalibrierungspersistenz im NVS (bestaetigt 2026-03-26)
- [ ] Rohdaten fuer definierte Referenzbewegungen systematisch dokumentieren

### Arbeitspaket 6.2 - Flex-Sensor-Basis

- [x] ADC-Pfad fuer den Flex-Sensor auslesen — GPIO1 ADC1, Spannungsteiler 10kOhm Pull-Down (bestaetigt 2026-03-22)
- [x] Rohwerte fuer gestreckte und gebeugte Fingerhaltung erfassen — gerade=1108, maximal gebogen=940, Bereich 168 Counts (bestaetigt 2026-03-22)
- [x] Messnotizen und erste Kalibrierfenster dokumentiert — `calibration/flex_sensor/MESSWERTE.md`, Formel biegung_prozent hinterlegt (bestaetigt 2026-03-22)

## Phase 7 - Kalibrierung und Mapping

### Arbeitspaket 7.1 - Referenzsystem

- [ ] Nullpunkt-Prozedur fuer IMUs festlegen
- [ ] Referenzpose fuer den Start sauber dokumentieren
- [ ] Achsinvertierungen und Segmentzuordnung pro Sensor pruefen
- [x] Persistenzstrategie fuer Kalibrierdaten festlegen — BNO055-Offsets werden im ESP32-NVS gespeichert und beim Boot geladen, Einzelkalibrierung per CAL0/CAL1/CAL2 (bestaetigt 2026-03-26)

### Arbeitspaket 7.2 - Gelenkwinkel

- [ ] relative Winkel zwischen den Segmenten berechnen
- [ ] Mapping auf Servo-Zielwinkel mit Grenzen dokumentieren
- [ ] Flex-Sensor-Mapping fuer den Greifer definieren
- [ ] Kalibrierergebnisse in den dafuer vorgesehenen Ordnern ablegen

## Phase 8 - Kommunikation, Servoausfuehrung und Safety

### Arbeitspaket 8.1 - Kommunikationskette

- [x] Peer-Setup und Adressverwaltung definiert — Unicast ESP-NOW, MAC in gitignorierter `peer_config.local.h`, Template committed (bestaetigt 2026-03-22)
- [x] Bench-Paketlayout v1 mit zwei IMUs abgeschlossen — historisch, abgeloest durch ImuPaket v3 (2026-03-26)
- [x] minimalen UART-Frame v1 zwischen Receiver und Arduino dokumentieren — festes Binaerformat mit Startbytes, Version, Sequenz, Flags, 5 Achs-Sollwerten und CRC8
- [x] Bench-Variante auf dritten IMU erweitert — ImuPaket v3 mit drei Sensoren und KalibStatus, Protokollversion 3 (bestaetigt 2026-03-26)
- [x] ImuPaket v4 mit Notaus-Flag (Bit 0 im `flags`-Feld), Toggle-Button an GPIO21, alle drei Firmwares aktualisiert (bestaetigt 2026-03-28)
- [ ] erste Arduino-Grundkette per UART aufbauen und validieren
- [ ] Kommunikationsrahmen danach von der Bench-Variante auf die dokumentierte Security-Baseline mit `session_id`, applikationsseitigem Authentisierungstag und Advisory-gepruefter Stack-Basis anheben

### Arbeitspaket 8.2 - Servoebene

- [ ] Servoinitialisierung fuer alle 5 Achsen umsetzen
- [ ] Gelenklimits pro Servo ausmessen und dokumentieren
- [ ] Rampen- oder Geschwindigkeitsbegrenzung einbauen
- [ ] Neutralposition fuer Fehlerfaelle festlegen

## Phase 9 - Integration, Latenz und Hardware-Verstetigung

### Arbeitspaket 9.1 - Gesamtsystem

- [ ] Ende-zu-Ende-Datenpfad mit allen Komponenten pruefen
- [ ] Mappingfehler, Achsendreher und Totzonen abstimmen
- [ ] Dauerlauftests mit Messnotizen dokumentieren

### Arbeitspaket 9.2 - Hardware-Verstetigung

- [ ] Sensorbefestigung und Kabelfuehrung fuer den Arm dokumentieren
- [ ] Receiver und Arduino am Arm ordentlich integrieren
- [ ] Stromversorgung, Befestigung und Entlastung finalisieren

## Phase 10 - Zukunftsausbau nach dem Grundsystem

### Arbeitspaket 10.1 - Mobile Plattform und Teleoperation

- [ ] fahrbaren oder bewegbaren Untergrund fuer das Gesamtsystem planen
- [ ] Dashboard-, Handy-App- und Controller-Steuerung fuer Fahrzeug und Roboterarm konzipieren
- [ ] Kamerasicht fuer das Gesamtsystem integrierbar vorbereiten

### Arbeitspaket 10.2 - Uebertragbare Aktuatorabstraktion

- [ ] Zwischenschicht zwischen Bewegungsinterpretation und direkter Servoausgabe definieren
- [ ] Aktuatoradapter fuer andere Servo- oder Robotiksysteme vorbereiten
- [ ] Zukunftsausbau unter `future/` dokumentieren und spaeter in aktive Bereiche ueberfuehren
