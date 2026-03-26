# Security Framework

Dieses Dokument definiert die verbindlichen Sicherheitsgrundlagen des Projekts.
Es beschreibt, wie das System gegen unautorisierte Steuerung, unsichere Schnittstellen, unbeabsichtigte Freigaben und unsauberen Secret-Umgang abgesichert werden soll.

## Sicherheitszielbild

Die erste Projektversion wird als lokal betriebenes Embedded-System behandelt:

- `ESP-NOW` ist der einzige vorgesehene Funkpfad zwischen den ESP32
- `UART` ist die interne Uebergabe zwischen Receiver und Arduino
- WLAN, Internetanbindung, Cloud, Web-API und Fernwartung sind in v1 ausdruecklich nicht Teil des Zielsystems

## Trennung zu Safety

- `Security` behandelt Angriffe, Missbrauch, unautorisierte Steuerung, Provisioning, Debugzugriff und Secret-Umgang.
- `Safety` behandelt physische Bewegungssicherheit wie Gelenklimits, Neutralposition, Rampen und Watchdog-Reaktion.
- Aenderungen mit Auswirkung auf beide Bereiche muessen in `SECURITY_FRAMEWORK.md` und `SAFETY_FRAMEWORK.md` gespiegelt werden.

## Schutzobjekte

- Bewegungsautoritaet ueber den Roboterarm
- Peer-Zuordnung zwischen Controller-ESP32 und Receiver-ESP32
- serielle Uebergabe an die Arduino-Servoebene
- Kalibrierungsdaten, Grenzwerte und Referenzzustaende
- spaetere Secret- oder Pairing-Daten
- Debug- und Provisioning-Zugaenge

## V1-Entscheidungen

- keine WLAN-Infrastruktur und keine Webschnittstellen dokumentieren
- keine produktiven Secrets im Repository ablegen
- keine offene Broadcast-Steuerung als Standardpfad zulassen
- keine unkontrollierte Debug- oder Provisioning-Schnittstelle fuer den Zielbetrieb vorsehen
- feste, dokumentierte Peer-Beziehungen statt dynamischer automatischer Kopplung anstreben

## Kommunikationssicherheit

- Funkkommunikation wird als dedizierter lokaler Steuerkanal behandelt
- unbekannte oder nicht freigegebene Peers duerfen keine Bewegungsdaten einspeisen
- Frames muessen Integritaets-, Versions- und Frischeannahmen unterliegen
- Replay- oder Duplikatverhalten muss als eigener Sicherheitsfall beschrieben werden
- Der eigentliche Security-Uplift wird bewusst erst nach drittem IMU und erster `Receiver -> Arduino`-Grundkette aktiviert; schon vorher muessen aber Versionsfelder und Erweiterungspunkte fuer spaetere Security-Felder mitgedacht werden

## ESP-NOW-Risikobasis

- `ESP-NOW` ist nicht automatisch sicher, nur weil es lokal und verbindungslos arbeitet.
- Laut Espressif ist Verschluesselung nur aktiv, wenn fuer Peers passende LMKs gesetzt sind; ohne gesetzte LMK bleiben Unicast-Frames unverschluesselt.
- Wenn die PMK nicht explizit gesetzt wird, verwendet die Plattform eine Default-PMK. Das ist fuer einen echten Steuerpfad nicht akzeptabel.
- Broadcast- oder Multicast-Steuerung ist fuer v1 kein Sicherheitsmodell, weil verschluesselte Multicast-Frames laut Espressif nicht unterstuetzt werden.
- MAC-seitiger Sendeerfolg ist nicht gleichbedeutend mit sicher akzeptierter Anwendungsnachricht; deshalb braucht der Projektpfad eigene ACK-, Session- und Frischelogik.

## Sicherheitskritische Folgerungen fuer dieses Projekt

- nur dedizierte Unicast-Beziehungen zwischen Controller und Receiver fuer Bewegungsdaten zulassen
- vor Bewegungsfreigabe explizit PMK, LMK, Peer-Allowlist und Kanalannahmen festlegen
- keine Sicherheitsentscheidung auf XOR, CRC oder reine Plausibilitaetspruefung stuetzen
- fuer den Anwendungsrahmen eigenen Session-Identifier, monotone Zaehler und einen applikationsseitigen Authentisierungstag vorsehen
- Empfangs- und Parsinglogik so planen, dass Laengen- und Strukturpruefungen vor jeder tieferen Verarbeitung greifen
- Schon in Bench-Paketen muessen `protokoll_version` und klar erkennbare Erweiterungspunkte fuer spaetere `flags`, Sensor-Gueltigkeit oder Auth-Felder mitgedacht werden, damit spaetere Security-Aktivierung keinen Komplettumbau erzwingt

## Abhaengigkeiten und Patch-Disziplin

- Vor der spaeteren Implementierung muss die konkrete `Arduino-ESP32`- bzw. `ESP-IDF`-Basis auf bekannte `ESP-NOW`-Advisories geprueft werden.
- Falls das Projekt die eigenstaendige Komponente `espressif/esp-now` nutzt, gilt zusaetzlich deren Advisory-Lage.
- Die committed Zielbasis fuer Bench und spaeteren Realbetrieb wird in `preparation/esp32_environment/README.md` gepflegt.
- Lokal geflashte Ist-Versionen und Freigabenotizen gehoeren zusaetzlich nach `security/local/stack_versions.local.md` oder in gleichwertige gitignorierte Dateien.
- Kein Bewegungsbetrieb auf einer bekannten verwundbaren `ESP-NOW`- oder `ESP-IDF`-Basis.

## Schnittstellensicherheit

- UART darf nur klar definierte, validierbare Bewegungsframes akzeptieren
- Debug- und Wartungsschnittstellen muessen als lokale Servicepfade beschrieben werden
- unsichere Generalschnittstellen wie Kommando-Shells oder unstrukturierte Fernkonfiguration sind fuer v1 nicht vorgesehen

## Secret- und Provisioning-Regeln

- echte Produktionswerte, Peer-Listen oder Sicherheitsschluessel kommen nicht ins Repo
- Beispielwerte muessen klar als Platzhalter markiert werden
- Provisioning wird als kontrollierter manueller Prozess fuer lokale Entwicklungsgeraete behandelt
- absolute lokale Pfade duerfen nicht in Skripten, Konfigurationsdateien oder Dokumenten eingecheckt werden — egal ob Linux, WSL oder Windows; diese gehoeren in gitignorierte lokale Konfigurationsdateien (z.B. `sync_config.local.sh`) mit generischen Platzhaltern im Template
- API-Schluessel, Zugangsdaten und IP-Adressen unterliegen denselben Regeln wie Secrets und muessen ebenfalls per `.gitignore` geblockt werden

## Nachweis und Pflege

- Detailregeln liegen unter `security/`
- technische Auswirkungen muessen in Kommunikations-, Architektur- und Testdokumente gespiegelt werden
- nach jedem groesseren Security-relevanten Schritt muessen Dokumente aktualisiert und `bash ./scripts/update_docs.sh` ausgefuehrt werden

## Recherchequellen

- [ESP-IDF ESP-NOW Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html) fuer CCMP, PMK/LMK, Broadcast-Grenzen und die Notwendigkeit von Anwendungs-ACKs und Sequenznummern.
- [Security Overview for espressif/esp-now](https://github.com/espressif/esp-now/security) fuer die aktuelle Advisory-Lage der optionalen `espressif/esp-now`-Komponente.
- [Replay Attacks Vulnerability In ESP-NOW](https://github.com/espressif/esp-now/security/advisories/GHSA-wf6q-c2xr-77xj) fuer die dokumentierte Replay-Problematik der Komponente bis `2.5.1`.
- [OOB Vulnerability In ESP-NOW Group Type Message](https://github.com/espressif/esp-now/security/advisories/GHSA-q6f6-4qc5-vhx5) fuer Speicherkorruptionsrisiken in der Gruppen-Nachrichtenverarbeitung der Komponente bis `2.5.1`.
- [ESP-NOW Integer Underflow Vulnerability Advisory](https://github.com/espressif/esp-idf/security/advisories/GHSA-hqhh-cp47-fv5g) fuer die Advisory-Lage im `ESP-IDF`-WLAN-/ESP-NOW-Stack.
