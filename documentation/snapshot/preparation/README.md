# preparation

## Zweck

Dieser Ordner enthaelt die vorbereitende Dokumentation fuer Toolchain, Bench, Entwicklungsumgebung und Inbetriebnahmebereitschaft.

## Aktueller Stand

Die Vorbereitungsbasis ist angelegt und teilweise bereits gegen echte Bench-Hardware verifiziert.
Toolchain, Boardfakten und erste Bench-Pfade sind dokumentiert; Aufbau-, Freigabe- und Gegencheckschritte werden hier weiter konsistent gehalten.

## Inhalt

- `arduino_ide_setup/` — Hauptumgebung fuer Arduino IDE, Upload, Boardeinstellungen und Alltagsworkflow
- `esp32_environment/` — allgemeiner ESP32-Umgebungsbereich mit bestaetigtem Boardstand, Bibliotheksbasis, Stack-Stand und PlatformIO als Fallback/Gegencheck
- `bench_setup/` fuer Arbeitsplatz, Strom, Verkabelungsdisziplin und Testreihenfolge
- `robot_arm_readiness/` fuer Voraussetzungen vor Aufbau und erster Inbetriebnahme

## Regeln

- keine Installationsskripte als Pflicht voraussetzen
- Vorbereitung als Checklisten- und Entscheidungsbasis dokumentieren
- Vorbereitungsfortschritt muss mit Hardware-, Security- und Firmwaredokumenten konsistent bleiben

## Schnittstellen/Abhaengigkeiten

- verweist auf `../hardware/`, `../firmware/`, `../security/` und `../docs/templates/`
