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

## Phase 2 - Security-Grundlage

### Arbeitspaket 2.1 - Sicherheitsrahmen

- [x] `SECURITY_FRAMEWORK.md` als Root-Dokument einfuehren
- [x] `security/` mit Bedrohungsmodell, Kommunikations- und Provisioning-Regeln anlegen
- [x] `ESP-NOW` als lokalen v1-Funkpfad ohne WLAN dokumentieren
- [x] lokale Secret- und Geraetedaten per `.gitignore` und `security/local/` absichern

### Arbeitspaket 2.2 - Security-Nachweisbasis

- [x] `tests/security/` als eigenen Nachweisbereich einfuehren
- [ ] Security-Szenarien spaeter in konkrete Testfaelle herunterbrechen

## Phase 3 - Vorbereitung und Toolchain

### Arbeitspaket 3.1 - Entwicklungsbasis

- [x] `preparation/` als eigenen Bereich fuer Vorbedingungen und Setup anlegen
- [x] Arduino IDE als ersten dokumentierten Toolchainpfad festhalten
- [x] ESP32-Umgebung, Bench und Readiness als Checklistenbereiche strukturieren
- [ ] reale Installations- und Verifikationsschritte spaeter nachziehen

### Arbeitspaket 3.2 - Vorbereitungsfreigabe

- [ ] Toolchain-Bereitschaft gegen Dokumentation pruefen
- [ ] Bench-Vorbedingungen und Sicherheitsfreigaben konkretisieren
- [ ] Readiness vor erster Hardware- oder Firmwarearbeit dokumentieren

## Phase 4 - Hardware-Readiness und Dokumentationsvorlagen

### Arbeitspaket 4.1 - Hardwaredokumentation

- [x] `hardware/assembly/` und `hardware/bringup/` als neue Unterbereiche einfuehren
- [x] Inventar-, Aufbau-, Verkabelungs- und Einschaltvorlagen unter `docs/templates/` anlegen
- [ ] reale Stueckliste, Verkabelung und Aufbauabweichungen spaeter nachziehen

### Arbeitspaket 4.2 - Nachweisstruktur

- [ ] Hardware-Inventar gegen realen Materialstand dokumentieren
- [ ] Aufbauablauf und Bringup-Reihenfolge mit echten Nachweisen fuellen
- [ ] Sensormontage und erste Stromtests sauber protokollieren

## Phase 5 - Architektur und Portabilitaetsfundament

### Arbeitspaket 5.1 - Schnittstellenklarheit

- [ ] Hardware-Abstraktionspunkte fuer Sensorik, Transport und Servoausgabe festziehen
- [ ] Konfigurations- und Kalibrierdaten sauber von harter Laufzeitlogik trennen
- [ ] Erweiterbarkeit auf andere Hardwarepfade fachlich vorbereiten

## Phase 6 - Sensorvalidierung

### Arbeitspaket 6.1 - IMU-Basis

- [ ] einzelne BNO055 am Controller lesen
- [ ] PCA9548A-Kanalumschaltung fuer mehrere IMUs testen
- [ ] gleichzeitiges Auslesen aller benoetigten IMUs validieren
- [ ] Rohdaten fuer Referenzbewegungen dokumentieren

### Arbeitspaket 6.2 - Flex-Sensor-Basis

- [ ] ADC-Pfad fuer den Flex-Sensor auslesen
- [ ] Rohwerte fuer gestreckte und gebeugte Fingerhaltung erfassen
- [ ] Messnotizen und erste Kalibrierfenster dokumentieren

## Phase 7 - Kalibrierung und Mapping

### Arbeitspaket 7.1 - Referenzsystem

- [ ] Nullpunkt-Prozedur fuer IMUs festlegen
- [ ] Referenzpose fuer den Start sauber dokumentieren
- [ ] Achsinvertierungen und Segmentzuordnung pro Sensor pruefen
- [ ] Persistenzstrategie fuer Kalibrierdaten festlegen

### Arbeitspaket 7.2 - Gelenkwinkel

- [ ] relative Winkel zwischen den Segmenten berechnen
- [ ] Mapping auf Servo-Zielwinkel mit Grenzen dokumentieren
- [ ] Flex-Sensor-Mapping fuer den Greifer definieren
- [ ] Kalibrierergebnisse in den dafuer vorgesehenen Ordnern ablegen

## Phase 8 - Kommunikation, Servoausfuehrung und Safety

### Arbeitspaket 8.1 - Kommunikationskette

- [ ] Peer-Setup und Adressverwaltung definieren
- [ ] Paketlayout mit Integritaets- und Frischeannahmen festziehen
- [ ] UART-Frame zwischen Receiver und Arduino abstimmen

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
