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
- `future/` enthaelt nur Zukunftsausbau, Post-v1-Ideen und spaeter zu aktivierende Erweiterungslinien.
- `documentation/` ist ausschliesslich generierter Snapshot-Inhalt und wird nicht manuell gepflegt.

## Code-Qualitaet und Portabilitaet

- Code muss modular, sauber abstrahierbar und langfristig skalierbar geschrieben werden.
- Fachlogik, Hardwarezugriff, Kommunikationsadapter, Sicherheitslogik und Konfigurationswissen muessen getrennt bleiben.
- Hardware-spezifische Annahmen duerfen nicht ungeordnet ueber mehrere Module verteilt werden.
- Das Projekt soll spaeter auf andere Hardware uebertragbar bleiben, daher muessen austauschbare Sensor-, Controller- und Transportpfade frueh mitgedacht werden.
- Neue Erweiterungen sollen vorzugsweise ueber klar benannte Module, Adapter oder Konfigurationspunkte anschliessen.

## Sprache im Code

- Variablennamen, Strukturnamen, Schichtbezeichnungen und Architekturelemente werden auf **Deutsch** verfasst.
- Funktionsnamen koennen deutsch oder englisch sein, muessen aber konsistent innerhalb eines Moduls bleiben.
- Pflicht-Deutsch: alle selbst definierten Variablen, Struct-Felder, Enums, Konstanten und Kommentare.
- Ausnahmen: Arduino/ESP-IDF-API-Bezeichner, Bibliothekstypen und erzwungene Konventionen externer Frameworks bleiben unveraendert.
- Bestehender Code muss nicht rueckwirkend umbenannt werden — die Regel gilt fuer alle neuen und geaenderten Dateien.

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
- In Firmware-Dateien darf nichts hartcodiert werden, was nicht zwingend eine Compile-Zeitkonstante sein muss. MACs, Schluessel, Peer-Adressen und Geraete-IDs gehoeren in gitignorierte lokale Konfigurationsdateien (z.B. `peer_config.local.h`). Die `.gitignore`-Muster fuer Firmware-Konfigurationen decken `firmware/**/peer_config.local.*`, `firmware/**/credentials.local.*` und `firmware/**/device_identity.local.*` ab.
- Vor jedem `git push` muss geprueft werden, ob in getrackte Dateien Secrets, Schluessel, MACs, IP-Adressen, Passwoerter oder sonstige sensible Werte eingeflossen sind, die nicht durch `.gitignore` geblockt werden. Ein Push darf erst erfolgen, wenn diese Pruefung abgeschlossen ist.
- Vor jedem `git push` muss geprueft werden, ob alle abgehakten Punkte `[x]` konsistent ueber alle Dokumente stimmen. `ROADMAP.md`, `PROJEKT_FORTSCHRITT.md`, `PROJEKT_ABLAUFPLAN.md` und lokale Roadmaps duerfen sich nicht widersprechen.
- Vor jedem `git push` muessen `README.md`-Abschnitte `Aktueller Fokus` und `Aktueller Entwicklungsstand` den echten Projektstand widerspiegeln.

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
- Forschungs-, Architektur-, Hardware-, Security-, Safety- und Entscheidungsdokumente muessen die verwendeten externen Quellen in einem eigenen Abschnitt `Recherchequellen` auffuehren.
- Jede eingetragene Quelle muss als Link nachvollziehbar sein und kurz erkennbar machen, wofuer sie im Dokument verwendet wurde.
- Externe technische Annahmen oder Empfehlungen duerfen nicht als feste Projektentscheidung stehen bleiben, wenn die Quelle dazu im betroffenen Dokument fehlt.
- Nach jedem groesseren Schritt muessen die betroffenen Dokumente direkt aktualisiert werden.
- Danach muss immer `bash ./scripts/update_docs.sh` ausgefuehrt werden, damit `documentation/` den aktuellen Stand spiegelt.
- Lokale `ROADMAP.md`-Dateien muessen mit der globalen `ROADMAP.md` konsistent bleiben.

## Zusammenarbeit mit KI

- Vor groesseren Aenderungen sind `README.md`, `CLAUDE.md`, `ROADMAP.md`, `PROJEKT_ABLAUFPLAN.md`, `PROJEKT_FORTSCHRITT.md` und `GLOBAL_RULES.md` zu lesen.
- Vor Architektur-, Kommunikations-, Security- oder Safety-Entscheidungen sind die Framework-Dokumente zusaetzlich zu lesen.
- Beim Wechsel in einen Unterordner ist die lokale `README.md` zu lesen, sofern der Inhalt des Ordners nicht bereits aus der laufenden Sitzung bekannt ist. Das gilt fuer alle KI-Einheiten und Projektmitarbeitenden gleichermassen.
- Nach jeder Kontextkomprimierung muessen `GLOBAL_RULES.md` und `SECURITY_FRAMEWORK.md` als erstes neu gelesen werden, bevor weitere Arbeit aufgenommen wird. Danach gelten die normalen Lesepflichten wie oben beschrieben.
- Wenn ausnahmsweise ein vorlaeufiger Workaround noetig ist, muss er als Zwischenstand kenntlich gemacht werden.
