# tests/bench

## Zweck

Dieser Ordner ist fuer kleine, fokussierte Teilpruefungen einzelner Komponenten vorgesehen.

## Aktueller Stand

Die Struktur ist vorbereitet, waehrend erste Bench-Ergebnisse bereits in Fach- und Messdokumenten vorliegen.
Diese Ergebnisse muessen noch in klar abgegrenzte Bench-Testfaelle ueberfuehrt werden.

## Inhalt

- Sensorlesetests
- Multiplexer- und I2C-Checks
- serielle Teilpruefungen einzelner Kommunikationsschritte

## Regeln

- Bench-Checks sollen klein, schnell und eindeutig sein
- jeder Check soll klar benennen, welche Komponente oder Schnittstelle geprueft wird

## Schnittstellen/Abhaengigkeiten

- kann auf `firmware/`, `calibration/` und `docs/measurement_logs/` verweisen
