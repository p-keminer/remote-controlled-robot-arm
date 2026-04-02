# tests

## Zweck

Dieser Ordner ist fuer nachvollziehbare Test- und Verifizierungsarbeit reserviert.

## Aktueller Stand

Die Teststruktur ist vorbereitet und erste Bench-Ergebnisse liegen bereits in Firmware-, Hardware- und Kalibrierdokumenten vor.
Diese Nachweise muessen noch systematisch in eigene Testfaelle fuer Bench, Security, Integration, Latenz und Safety ueberfuehrt werden.

## Inhalt

- `bench/` fuer kleine Teilpruefungen einzelner Komponenten
- `security/` fuer Angriffs-, Missbrauchs- und Integritaetsfaelle
- `integration/` fuer Ende-zu-Ende-Ablaufpruefungen
- `latency/` fuer Zeitverhalten und Messmethoden
- `safety/` fuer Grenz-, Stop- und Fehlerfallpruefungen

## Regeln

- Tests sollen reproduzierbar und klar dokumentiert sein
- Messmethode, erwartetes Verhalten und Ergebnis muessen zusammen sichtbar sein
- Security- und Safety-Nachweise muessen sichtbar getrennt bleiben

## Schnittstellen/Abhaengigkeiten

- orientiert sich an `../SECURITY_FRAMEWORK.md`, `../SAFETY_FRAMEWORK.md`, `../COMMUNICATION_FRAMEWORK.md` und `../CALIBRATION_FRAMEWORK.md`
- verweist fuer reale Messungen auf `../docs/measurement_logs/`
