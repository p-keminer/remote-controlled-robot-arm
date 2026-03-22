# AGENTS.md

Dieses Dokument ist fuer KI-Agenten und automatisierte Entwicklungshelfer gedacht.
Es beschreibt den Projektkontext und die verbindliche Arbeitsweise.

## Projektidentitaet

Projektname: `IMU Robotic Arm`

Kurzbeschreibung:
Ein 5-DOF-Roboterarm soll ueber Armbewegungen, mehrere BNO055-IMUs und einen Flex-Sensor lokal drahtlos gesteuert werden.
Die Architektur trennt Vorbereitung, Security, Hardware, Kalibrierung, Firmware und Testnachweise klar voneinander.

## Aktuelle Projektphase

Aktuell befindet sich das Projekt in Phase 6/8 — Sensorvalidierung abgeschlossen, Kommunikationskette steht, naechster Schritt Hardware-Aufbau:

- Toolchain steht: Arduino IDE 3.3.7 + PlatformIO (bestaetigt 2026-03-22)
- BNO055 Einzel + Dual via PCA9548A-Mux validiert (bestaetigt 2026-03-22)
- Flex-Sensor ADC-Pfad ausgelesen und kalibriert (bestaetigt 2026-03-22)
- ESP-NOW Unicast mit ImuPaket v1 laeuft (bestaetigt 2026-03-22)
- Naechste Schritte: LED-Debugging, Buzzer, Roboterarm-Aufbau, UART zu Arduino

## Pflicht nach Kontextkomprimierung

Nach jeder Kontextkomprimierung oder beim Sitzungsstart ohne vollstaendigen Verlauf gilt zwingend:

1. `GLOBAL_RULES.md` lesen
2. `SECURITY_FRAMEWORK.md` lesen
3. Danach die normalen Lesepflichten unten einhalten

## Verbindliche Arbeitsweise fuer Agenten

- Vor jeder groesseren Aenderung `README.md`, `CLAUDE.md`, `ROADMAP.md`, `PROJEKT_ABLAUFPLAN.md`, `PROJEKT_FORTSCHRITT.md` und `GLOBAL_RULES.md` lesen.
- Vor Produkt-, System-, Security- oder Safety-Entscheidungen zusaetzlich `ARCHITECTURE.md`, `SYSTEM_FRAMEWORKS.md`, `COMMUNICATION_FRAMEWORK.md`, `SECURITY_FRAMEWORK.md`, `CALIBRATION_FRAMEWORK.md` und `SAFETY_FRAMEWORK.md` lesen.
- Vor Aenderungen in `preparation/`, `security/`, `firmware/`, `hardware/`, `calibration/`, `tests/`, `docs/` oder `future/` immer die lokale `README.md` und gegebenenfalls `ROADMAP.md` lesen.
- Beim Wechsel in einen Unterordner ist die lokale `README.md` zu lesen, sofern der Inhalt nicht bereits aus der laufenden Sitzung bekannt ist.
- Nach strukturellen Aenderungen immer die zugehoerigen lokalen Dokumente aktualisieren.
- Bei forschungs- oder entscheidungsgetriebenen Dokumenten alle verwendeten externen Quellen im betroffenen Dokument unter `Recherchequellen` eintragen.
- Nach Aenderungen an Dokumenten den Sammelstand mit `bash ./scripts/update_docs.sh` erneuern.
- Die globale `ROADMAP.md` und die lokale Roadmap des betroffenen Unterprojekts muessen synchron gehalten werden.
- Nicht-repotaugliche lokale Werte nur in `security/local/` und niemals in tracked Doku oder Konfigurationen ablegen.
- Kommunikation, Servoausfuehrung, Kalibrierung, Security, Hardware und Testnachweise strikt getrennt halten.
- In Firmware-Dateien nichts hartcodieren was nicht zwingend hartcodiert sein muss. MACs, Schluessel und Peer-Adressen gehoeren in gitignorierte lokale Header (`peer_config.local.h` o.ae.).
- Vor jedem `git push` pruefen, ob in getrackte Dateien Secrets, Schluessel, MACs, IP-Adressen oder andere sensible Werte eingeflossen sind, die nicht durch `.gitignore` geblockt werden. Push nur nach abgeschlossener Pruefung.
- Vor jedem `git push` pruefen, ob alle abgehakten Punkte `[x]` konsistent ueber alle Dokumente stimmen — `ROADMAP.md`, `PROJEKT_FORTSCHRITT.md`, `PROJEKT_ABLAUFPLAN.md` und lokale Roadmaps duerfen sich nicht widersprechen.
- Vor jedem `git push` die `README.md`-Abschnitte `Aktueller Fokus` und `Aktueller Entwicklungsstand` auf echten Projektstand pruefen und bei Bedarf aktualisieren.

## Verbindliche Designentscheidungen

- `ESP-NOW` ist in v1 der Funkstandard zwischen Controller und Receiver.
- WLAN ist in v1 ausdruecklich nicht Teil des Zielsystems.
- Der Receiver-ESP32 ist Bruecke zwischen Funk und Arduino, nicht die eigentliche Servo-Steuerinstanz.
- Der Arduino ist fuer die direkte Servo-Ansteuerung mit Limits, Rampen und Neutralverhalten zustaendig.
- Relative Winkel zwischen Armsegmenten sind der bevorzugte fachliche Ausgangspunkt fuer das Gelenkmapping.
- Security und Safety werden als getrennte Dokumentations- und Entscheidungsbereiche behandelt.
- Der reale Aufbau wird in dieser Ausbaustufe nur als Ablauf, Checklisten- und Nachweisstruktur dokumentiert.

## Prioritaeten

1. stabile, nachvollziehbare Struktur
2. dokumentierte Security-, Safety- und Kommunikationsregeln
3. modularer, abstrahierbarer und auf andere Hardware uebertragbarer Aufbau
4. saubere Vorbereitung und nachvollziehbare Hardwaredokumentation
5. quellenbasierte und nachvollziehbare Entscheidungsdokumentation
6. erst danach mehr Komfort und Zusatzfunktionen

## Wenn Unklarheit entsteht

Im Zweifel ist immer die einfachere, modularere, sicherere und besser dokumentierbare Loesung zu bevorzugen.
