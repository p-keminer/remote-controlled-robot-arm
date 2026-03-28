# Globale Entwicklungsregeln

Dieses Dokument ist fuer Menschen und KI gleichermassen verbindlich.

## Struktur und Verantwortungen

- `preparation/` enthaelt nur vorbereitende Ablaufe, Checklisten und Toolchain-Dokumentation.
- `security/` enthaelt nur Security-Dokumentation, Bedrohungsmodell und Regeln fuer Schnittstellen, Provisioning und lokale Identitaeten.
- `firmware/` enthaelt nur Laufzeitlogik fuer Controller, Receiver, Bridge und Arduino-Ausfuehrung.
- `hardware/` enthaelt nur Schaltplaene, Verdrahtung, Aufbauablauf, Bringup und Stuecklisten.
- `calibration/` enthaelt nur Kalibrierregeln, Referenzdaten und Grenzwerte.
- `tests/` enthaelt Testablaeufe, Bench-Skripte und Nachweise fuer Security, Safety und Verhalten.
- `docs/` bleibt manuell gepflegte Arbeits-, Nachweis- und Template-Dokumentation.
- `dashboard/` enthaelt nur Entwicklungs-Dashboard-Views, MQTT MCP Server und Debug-Tooling (Bench-Werkzeug, nicht Teil des v1-Steuerpfads).
- `future/` enthaelt nur Zukunftsausbau, Post-v1-Ideen und spaeter zu aktivierende Erweiterungslinien.
- `official_downloads/` enthaelt nur importierte Herstellerstaende, Vendor-Artefakte und projektrelevante Auswertungen dazu.
- `documentation/` ist ausschliesslich generierter Snapshot-Inhalt und wird nicht manuell gepflegt. Nach jedem grossen Dokumentationsdurchlauf muss `documentation/snapshot/` komplett neu aus den Live-Dateien generiert werden.

## Code-Qualitaet und Portabilitaet

- Code muss modular, sauber abstrahierbar und langfristig skalierbar geschrieben werden.
- Fachlogik, Hardwarezugriff, Kommunikationsadapter, Sicherheitslogik und Konfigurationswissen muessen getrennt bleiben.
- Hardware-spezifische Annahmen duerfen nicht ungeordnet ueber mehrere Module verteilt werden.
- Das Projekt soll spaeter auf andere Hardware uebertragbar bleiben, daher muessen austauschbare Sensor-, Controller- und Transportpfade frueh mitgedacht werden.
- Neue Erweiterungen sollen vorzugsweise ueber klar benannte Module, Adapter oder Konfigurationspunkte anschliessen.

## Lizenz und Weitergabe

- Die projekt-eigenen Inhalte dieses Repositories stehen unter `LICENSE.md` auf Basis der Apache License 2.0.
- Sicherheits- und Einsatzhinweise stehen getrennt in `SAFETY_NOTICE.md` und veraendern die Apache-2.0-Lizenz nicht.
- Die angestrebte Portabilitaet des Projekts ist ein Architekturziel und keine technische oder sicherheitsrechtliche Freigabe fuer beliebige Robotiksysteme.
- Hersteller- und Drittmaterialien, insbesondere unter `official_downloads/`, behalten ihre eigenen Rechte und Hinweise und werden durch die Projektlizenz nicht umgelabelt.
- Bei Dokumenten oder Dateien mit abweichenden Rechten muss diese Abweichung lokal klar sichtbar gemacht werden.

## Sprache im Code

- Variablennamen, Strukturnamen, Schichtbezeichnungen und Architekturelemente werden auf **Deutsch** verfasst.
- Funktionsnamen koennen deutsch oder englisch sein, muessen aber konsistent innerhalb eines Moduls bleiben.
- Pflicht-Deutsch: alle selbst definierten Variablen, Struct-Felder, Enums, Konstanten und Kommentare.
- Ausnahmen: Arduino/ESP-IDF-API-Bezeichner, Bibliothekstypen und erzwungene Konventionen externer Frameworks bleiben unveraendert.
- Bestehender Code muss nicht rueckwirkend umbenannt werden — die Regel gilt fuer alle neuen und geaenderten Dateien.

## Board- und Flash-Konfiguration

- Alle drei ESP32-S3-WROOM-1-N16R8 muessen mit dem Custom Board `esp32:esp32:robotic_arm_s3n16r8` geflasht werden.
- Die Board-Definition liegt in `boards.local.txt` im lokalen Arduino-Paketordner (nicht im Repo — wird bei Toolchain-Setup einmalig eingerichtet, siehe `preparation/esp32_environment/README.md`).
- **NIEMALS** das generische Board `esp32:esp32:esp32s3` verwenden — dort ist `CDCOnBoot` deaktiviert, was bei diesen Boards einen permanenten Reset-Loop verursacht.
- Kritische Einstellungen im Custom Board: `cdc_on_boot=1`, `flash_size=16MB`, `psram_type=opi`, `memory_type=qio_opi`, `partitions=app3M_fat9M_16MB`.
- Vor jedem Flash-Vorgang die FQBN pruefen. Bei Unsicherheit die `boards.local.txt` konsultieren.
- COM-Port-Zuordnungen sind nicht fest und koennen sich bei jedem Einstecken aendern; immer per `arduino-cli board list` oder Geraete-Manager pruefen.

## Kommunikations- und Systemregeln

- `ESP-NOW` ist in v1 der einzige vorgesehene Funkpfad im Steuerpfad (Controller → Receiver → Arduino).
- WiFi wird ausschliesslich zwischen Bridge-ESP32 und Pi fuer das Entwicklungs-Dashboard genutzt.
- WLAN, Cloud und Web-Interfaces sind im Steuerpfad kein Zielpfad.
- Alle ESPs muessen auf dem gleichen WiFi-Kanal laufen (aktuell Kanal 1 = Router-Kanal).
- Receiver-Firmware validiert und uebergibt Bewegungsdaten, enthaelt aber keine offene Fernsteuer- oder Serviceschnittstelle.
- Arduino-Firmware ist fuer Servoausfuehrung, Limits, Rampen und Neutralverhalten zustaendig.
- Paket- und Protokollaenderungen muessen immer auch in `COMMUNICATION_FRAMEWORK.md` dokumentiert werden.

## Security und lokale Daten

- `Security` und `Safety` werden getrennt dokumentiert.
- Nicht-repotaugliche Daten wie Schluessel, lokale Geraeteidentitaeten, Peer-Listen oder Provisioning-Notizen gehoeren nur nach `security/local/`.
- Diese lokalen Dateien muessen ueber `.gitignore` blockiert bleiben.
- Echte Secrets, produktive IDs oder Peer-Werte duerfen nicht in Dokumente oder Beispielkonfigurationen eingecheckt werden.
- Debug- und Servicezugaenge muessen als lokale Entwicklungswege behandelt und klar vom Zielbetrieb getrennt werden.
- In Firmware-Dateien darf nichts hartcodiert werden, was nicht zwingend eine Compile-Zeitkonstante sein muss. MACs, Schluessel, Peer-Adressen, WiFi-Zugangsdaten und Geraete-IDs gehoeren in gitignorierte lokale Konfigurationsdateien (z.B. `peer_config.local.h`, `wifi_config.local.h`). Die `.gitignore`-Muster decken `firmware/**/peer_config.local.*`, `firmware/**/credentials.local.*`, `firmware/**/device_identity.local.*`, `firmware/**/packet_auth.local.*` und `firmware/**/wifi_config.local.*` ab. MQTT- und OTA-Credentials gehoeren in `dashboard/mcp/*.local.*`.
- Die committed Zielbasis fuer den Stack wird in `preparation/esp32_environment/README.md` gepflegt; lokal geflashte Ist-Versionen und Freigabenotizen gehoeren nach `security/local/stack_versions.local.md` oder vergleichbare gitignorierte Dateien.
- Skripte duerfen keine nutzer- oder systemspezifischen absoluten Pfade enthalten. Konfigurierbare Pfade gehoeren in gitignorierte lokale Konfigurationsdateien (z.B. `sync_config.local.sh`). Committed werden nur Templates mit generischen Platzhaltern wie `/PFAD/ZUM/REPO/` — kein Hinweis auf Betriebssystem, Benutzername, Verzeichnisstruktur oder Toolchain.
- Vor jedem `git push` muss `bash scripts/secret_scan.sh --tracked` ausgefuehrt werden. Der Scanner prueft 10 Kategorien: private IPs, MAC-Adressen, Hex-MAC-Arrays, Passwoerter, WiFi-SSIDs, absolute Pfade, SSH-Daten, private Keys, hartcodierte Ports und getrackte .local-Dateien. Pre-Commit und Pre-Push Git-Hooks fuehren den Scan automatisch aus. Ein Push darf erst erfolgen, wenn alle Kategorien sauber sind.
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
- `documentation/` spiegelt nur die projektgepflegte Markdown-Quelldokumentation; externe Vendor-Archive wie `official_downloads/raw/` und `official_downloads/extracted/` bleiben davon ausgenommen.
- Lokale `ROADMAP.md`-Dateien muessen mit der globalen `ROADMAP.md` konsistent bleiben.

## Zusammenarbeit mit KI

- Vor groesseren Aenderungen sind `README.md`, `CLAUDE.md`, `ROADMAP.md`, `PROJEKT_ABLAUFPLAN.md`, `PROJEKT_FORTSCHRITT.md` und `GLOBAL_RULES.md` zu lesen.
- Vor Architektur-, Kommunikations-, Security- oder Safety-Entscheidungen sind die Framework-Dokumente zusaetzlich zu lesen.
- Beim Wechsel in einen Unterordner ist die lokale `README.md` zu lesen, sofern der Inhalt des Ordners nicht bereits aus der laufenden Sitzung bekannt ist. Das gilt fuer alle KI-Einheiten und Projektmitarbeitenden gleichermassen.
- Nach jeder Kontextkomprimierung muessen `GLOBAL_RULES.md` und `SECURITY_FRAMEWORK.md` als erstes neu gelesen werden, bevor weitere Arbeit aufgenommen wird. Danach gelten die normalen Lesepflichten wie oben beschrieben.
- Wenn ausnahmsweise ein vorlaeufiger Workaround noetig ist, muss er als Zwischenstand kenntlich gemacht werden.
