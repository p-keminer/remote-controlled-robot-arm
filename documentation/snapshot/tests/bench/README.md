# tests/bench

## Zweck

Dieser Ordner ist fuer kleine, fokussierte Teilpruefungen einzelner Komponenten vorgesehen.

## Aktueller Stand

Die Struktur ist vorbereitet, konkrete Bench-Checks folgen spaeter.

## Inhalt

- Sensorlesetests
- Multiplexer- und I2C-Checks
- serielle Teilpruefungen einzelner Kommunikationsschritte

## Regeln

- Bench-Checks sollen klein, schnell und eindeutig sein
- jeder Check soll klar benennen, welche Komponente oder Schnittstelle geprueft wird

## Schnittstellen/Abhaengigkeiten

- kann auf `firmware/`, `calibration/` und `docs/measurement_logs/` verweisen
