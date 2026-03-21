# AGENTS.md

Dieses Dokument ist fuer KI-Agenten und automatisierte Entwicklungshelfer gedacht.
Es beschreibt den Projektkontext und die verbindliche Arbeitsweise.

## Projektidentitaet

Projektname: `IMU Robotic Arm`

Kurzbeschreibung:
Ein 5-DOF-Roboterarm soll ueber Armbewegungen, mehrere BNO055-IMUs und einen Flex-Sensor lokal drahtlos gesteuert werden.
Die Architektur trennt Vorbereitung, Security, Hardware, Kalibrierung, Firmware und Testnachweise klar voneinander.

## Aktuelle Projektphase

Aktuell befindet sich das Projekt in einer dokumentationsgetriebenen Vorbereitungsphase:

- Struktur und Regeln festziehen
- Security- und Vorbereitungsschicht aufbauen
- hardwareseitige Dokumentation und Vorlagen vorbereiten
- Teilbereiche fuer spaetere Firmware- und Inbetriebnahmephasen sauber vorbereiten

## Verbindliche Arbeitsweise fuer Agenten

- Vor jeder groesseren Aenderung `README.md`, `CLAUDE.md`, `ROADMAP.md`, `PROJEKT_ABLAUFPLAN.md`, `PROJEKT_FORTSCHRITT.md` und `GLOBAL_RULES.md` lesen.
- Vor Produkt-, System-, Security- oder Safety-Entscheidungen zusaetzlich `ARCHITECTURE.md`, `SYSTEM_FRAMEWORKS.md`, `COMMUNICATION_FRAMEWORK.md`, `SECURITY_FRAMEWORK.md`, `CALIBRATION_FRAMEWORK.md` und `SAFETY_FRAMEWORK.md` lesen.
- Vor Aenderungen in `preparation/`, `security/`, `firmware/`, `hardware/`, `calibration/`, `tests/` oder `docs/` immer die lokale `README.md` und gegebenenfalls `ROADMAP.md` lesen.
- Nach strukturellen Aenderungen immer die zugehoerigen lokalen Dokumente aktualisieren.
- Nach Aenderungen an Dokumenten den Sammelstand mit `bash ./scripts/update_docs.sh` erneuern.
- Die globale `ROADMAP.md` und die lokale Roadmap des betroffenen Unterprojekts muessen synchron gehalten werden.
- Nicht-repotaugliche lokale Werte nur in `security/local/` und niemals in tracked Doku oder Konfigurationen ablegen.
- Kommunikation, Servoausfuehrung, Kalibrierung, Security, Hardware und Testnachweise strikt getrennt halten.

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
5. erst danach mehr Komfort und Zusatzfunktionen

## Wenn Unklarheit entsteht

Im Zweifel ist immer die einfachere, modularere, sicherere und besser dokumentierbare Loesung zu bevorzugen.
