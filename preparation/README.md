# preparation

## Zweck

Dieser Ordner enthaelt die vorbereitende Dokumentation fuer Toolchain, Bench, Entwicklungsumgebung und Inbetriebnahmebereitschaft.

## Aktueller Stand

Die Vorbereitung wird hier erstmals als eigener Bereich beschrieben.
Der Fokus liegt auf nachvollziehbaren Checklisten und nicht auf automatischer Installation.

## Inhalt

- `esp32_environment/` — **primaerer Toolchain-Pfad**: PlatformIO auf Windows, bestaetigter Boardstand, Bibliotheksbasis und Readiness-Checkliste
- `arduino_ide_setup/` — Fallback-Pfad fuer Arduino IDE; nicht der primaere Entwicklungsweg
- `bench_setup/` fuer Arbeitsplatz, Strom, Verkabelungsdisziplin und Testreihenfolge
- `robot_arm_readiness/` fuer Voraussetzungen vor Aufbau und erster Inbetriebnahme

## Regeln

- keine Installationsskripte als Pflicht voraussetzen
- Vorbereitung als Checklisten- und Entscheidungsbasis dokumentieren
- Vorbereitungsfortschritt muss mit Hardware-, Security- und Firmwaredokumenten konsistent bleiben

## Schnittstellen/Abhaengigkeiten

- verweist auf `../hardware/`, `../firmware/`, `../security/` und `../docs/templates/`
