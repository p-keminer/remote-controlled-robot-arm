# AGENTS.md

Dieses Dokument ist fuer KI-Agenten und automatisierte Entwicklungshelfer gedacht.
Es beschreibt den Projektkontext und die verbindliche Arbeitsweise.

## Projektidentitaet

Projektname: `IMU Robotic Arm`

Kurzbeschreibung:
Ein 5-DOF-Roboterarm soll ueber Armbewegungen, mehrere BNO055-IMUs und einen Flex-Sensor lokal drahtlos gesteuert werden.
Die Architektur trennt Vorbereitung, Security, Hardware, Kalibrierung, Firmware und Testnachweise klar voneinander.

## Aktuelle Projektphase

Aktuell befindet sich das Projekt in Phase 6/8 — Sensor- und Kommunikations-Bench ist validiert, Hardware-Aufbau, UART-Kette und Security-Haertung sind noch offen:

- Toolchain steht: Arduino IDE 3.3.7 als Hauptumgebung, PlatformIO als Fallback und Gegencheck (bestaetigt 2026-03-22)
- BNO055 drei Sensoren (Oberarm/Unterarm/Hand) via PCA9548A-Mux validiert, Kalibrierungsoffsets im NVS persistent (bestaetigt 2026-03-26)
- Flex-Sensor ADC-Pfad ausgelesen und kalibriert mit Live-Plausibilitaetspruefung (bestaetigt 2026-03-26)
- ESP-NOW Unicast mit `ImuPaket v3` (drei IMUs, KalibStatus, NVS-Persistenz) laeuft (bestaetigt 2026-03-26)
- LED-Debugging bench-validiert: Controller Ampelsystem + COMMS + FAULT, Receiver LINK + UART + FAULT (bestaetigt 2026-03-26)
- Bridge-ESP32: ESP-NOW Empfang → WiFi/MQTT → Mosquitto (Pi) mit OTA und Passwort-Auth (bestaetigt 2026-03-27)
- WiFi-Kanal 1 auf allen ESPs fuer ESP-NOW/WiFi-Koexistenz (bestaetigt 2026-03-28)
- MQTT MCP Server fuer Claude Live-Sensorzugriff mit 6 Tools (bestaetigt 2026-03-28)
- Kompletter Datenpfad validiert: Controller → ESP-NOW → Bridge → MQTT → Pi → MCP → Claude
- Secret-Scanner mit 10 Kategorien, Pre-Commit/Pre-Push Hooks (bestaetigt 2026-03-27)
- Adeept 5-DOF Roboterarm mechanisch im Stock-Zustand aufgebaut (bestaetigt 2026-03-24)
- Naechste Schritte: Stock-Baseline-Test, UART-Pfad Receiver → Arduino, Security-Uplift

## Pflicht nach Kontextkomprimierung oder Sitzungsstart

Nach jeder Kontextkomprimierung oder beim Sitzungsstart ohne vollstaendigen Verlauf gilt zwingend:

1. `MEMORY.md` lesen — enthaelt akkumulierten Projektstand, Hardware-Fakten und verbindliche Feedback-Regeln aus allen bisherigen Sessions
2. `GLOBAL_RULES.md` lesen
3. `SECURITY_FRAMEWORK.md` lesen
4. Danach die normalen Lesepflichten unten einhalten

`MEMORY.md` ist das zentrale Gedaechtnis des Projekts und gilt gleichermassen fuer Claude, Codex und alle anderen KI-Einheiten.

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
- Die Bridge (ESP-NOW → WiFi → MQTT → Pi) ist reines Entwicklungswerkzeug, nicht Teil des Steuerpfads.

## Board- und Flash-Konfiguration (KRITISCH)

Alle drei ESP32-S3-WROOM-1-N16R8 muessen mit dem Custom Board geflasht werden:

```
FQBN: esp32:esp32:robotic_arm_s3n16r8
```

**NIEMALS** das generische Board `esp32:esp32:esp32s3` verwenden — `CDCOnBoot` ist dort deaktiviert, was einen permanenten Reset-Loop verursacht.

Die Board-Definition liegt in `boards.local.txt` im lokalen Arduino-Paketordner (siehe `preparation/esp32_environment/README.md`). Kritische Einstellungen: `cdc_on_boot=1`, `flash_size=16MB`, `psram_type=opi`, `partitions=app3M_fat9M_16MB`.

Vor jedem Flash-Vorgang die FQBN pruefen. COM-Ports koennen sich bei jedem Einstecken aendern — immer per `arduino-cli board list` pruefen.

## Prioritaeten

1. stabile, nachvollziehbare Struktur
2. dokumentierte Security-, Safety- und Kommunikationsregeln
3. modularer, abstrahierbarer und auf andere Hardware uebertragbarer Aufbau
4. saubere Vorbereitung und nachvollziehbare Hardwaredokumentation
5. quellenbasierte und nachvollziehbare Entscheidungsdokumentation
6. erst danach mehr Komfort und Zusatzfunktionen

## Wenn Unklarheit entsteht

Im Zweifel ist immer die einfachere, modularere, sicherere und besser dokumentierbare Loesung zu bevorzugen.
