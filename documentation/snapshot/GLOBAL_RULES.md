# Globale Entwicklungsregeln

Dieses Dokument ist fuer Menschen und KI gleichermassen verbindlich.

## Struktur und Verantwortungen

- `preparation/` enthaelt nur vorbereitende Ablaufe, Checklisten und Toolchain-Dokumentation.
- `security/` enthaelt nur Security-Dokumentation, Bedrohungsmodell und Regeln fuer Schnittstellen, Provisioning und lokale Identitaeten.
- `firmware/` enthaelt nur Laufzeitlogik fuer Controller, Receiver und Arduino-Ausfuehrung.
- `hardware/` enthaelt nur Schaltplaene, Verdrahtung, Aufbauablauf, Bringup und Stuecklisten.
- `calibration/` enthaelt nur Kalibrierregeln, Referenzdaten und Grenzwerte.
- `tests/` enthaelt Testablaeufe, Bench-Skripte und Nachweise fuer Security, Safety und Verhalten.
- `docs/` bleibt manuell gepflegte Arbeits-, Nachweis- und Template-Dokumentation.
- `documentation/` ist ausschliesslich generierter Snapshot-Inhalt und wird nicht manuell gepflegt.

## Code-Qualitaet und Portabilitaet

- Code muss modular, sauber abstrahierbar und langfristig skalierbar geschrieben werden.
- Fachlogik, Hardwarezugriff, Kommunikationsadapter, Sicherheitslogik und Konfigurationswissen muessen getrennt bleiben.
- Hardware-spezifische Annahmen duerfen nicht ungeordnet ueber mehrere Module verteilt werden.
- Das Projekt soll spaeter auf andere Hardware uebertragbar bleiben, daher muessen austauschbare Sensor-, Controller- und Transportpfade frueh mitgedacht werden.
- Neue Erweiterungen sollen vorzugsweise ueber klar benannte Module, Adapter oder Konfigurationspunkte anschliessen.

## Kommunikations- und Systemregeln

- `ESP-NOW` ist in v1 der einzige vorgesehene Funkpfad zwischen den ESP32.
- WLAN, Cloud und Web-Interfaces sind in v1 kein Zielpfad.
- Receiver-Firmware validiert und uebergibt Bewegungsdaten, enthaelt aber keine offene Fernsteuer- oder Serviceschnittstelle.
- Arduino-Firmware ist fuer Servoausfuehrung, Limits, Rampen und Neutralverhalten zustaendig.
- Paket- und Protokollaenderungen muessen immer auch in `COMMUNICATION_FRAMEWORK.md` dokumentiert werden.

## Security und lokale Daten

- `Security` und `Safety` werden getrennt dokumentiert.
- Nicht-repotaugliche Daten wie Schluessel, lokale Geraeteidentitaeten, Peer-Listen oder Provisioning-Notizen gehoeren nur nach `security/local/`.
- Diese lokalen Dateien muessen ueber `.gitignore` blockiert bleiben.
- Echte Secrets, produktive IDs oder Peer-Werte duerfen nicht in Dokumente oder Beispielkonfigurationen eingecheckt werden.
- Debug- und Servicezugaenge muessen als lokale Entwicklungswege behandelt und klar vom Zielbetrieb getrennt werden.

## Kalibrierung und Daten

- Referenzposen, Invertierungen, Min-/Max-Grenzen und Mappingannahmen werden dokumentiert und versioniert.
- Kalibrierdaten muessen nachvollziehbar einem Sensor, einer Achse oder einem Servo zuordenbar sein.
- Rohmessungen, daraus abgeleitete Grenzwerte und spaetere Korrekturentscheidungen werden nicht vermischt.

## Safety

- Kein neuer Bewegungsablauf ohne definierte Gelenklimits.
- Fehlerfaelle fuer Paketverlust, ungueltige Daten oder Sensorstillstand muessen vor echtem Bewegungsbetrieb beschrieben sein.
- Neutralpositionen, Stopplogik und Watchdog-Verhalten duerfen nicht nur implizit angenommen werden.
- Sicherheitskritische Aenderungen muessen die betroffenen Test- und Kalibrierdokumente mitziehen.

## Dokumentation

- Jeder relevante Ordner braucht mindestens eine `README.md`.
- Jedes groessere Unterprojekt braucht eine `ROADMAP.md`.
- `PROJEKT_ABLAUFPLAN.md` fuehrt die Reihenfolge der Projektphasen und `PROJEKT_FORTSCHRITT.md` den aktuellen Managementstand.
- Verhalten, Datenfluss, Schnittstellen oder Arbeitsablaeufe muessen in den passenden Dokumenten nachgezogen werden.
- Nach jedem groesseren Schritt muessen die betroffenen Dokumente direkt aktualisiert werden.
- Danach muss immer `bash ./scripts/update_docs.sh` ausgefuehrt werden, damit `documentation/` den aktuellen Stand spiegelt.
- Lokale `ROADMAP.md`-Dateien muessen mit der globalen `ROADMAP.md` konsistent bleiben.

## Zusammenarbeit mit KI

- Vor groesseren Aenderungen sind `README.md`, `CLAUDE.md`, `ROADMAP.md`, `PROJEKT_ABLAUFPLAN.md`, `PROJEKT_FORTSCHRITT.md` und `GLOBAL_RULES.md` zu lesen.
- Vor Architektur-, Kommunikations-, Security- oder Safety-Entscheidungen sind die Framework-Dokumente zusaetzlich zu lesen.
- Vor lokalen Aenderungen ist die Dokumentation des betroffenen Unterordners zu lesen.
- Wenn ausnahmsweise ein vorlaeufiger Workaround noetig ist, muss er als Zwischenstand kenntlich gemacht werden.
