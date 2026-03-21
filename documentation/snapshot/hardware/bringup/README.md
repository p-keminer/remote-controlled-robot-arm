# hardware/bringup

## Zweck

Dieser Ordner beschreibt Vorpruefungen, sichere Erstinbetriebnahme und die Reihenfolge erster Hardwaretests.

## Aktueller Stand

Die Bringup-Dokumentation ist vorbereitet.
Die reale Inbetriebnahme folgt spaeter entlang der hier beschriebenen Struktur.

## Inhalt

- Vorpruefungen vor erstem Einschalten
- Strom- und Segmentfreigaben fuer schrittweise Tests
- Bezug zu Safety-, Security- und Bench-Regeln

## Regeln

- keine Erstinbetriebnahme ohne dokumentierte Vorbedingungen
- jede kritische Beobachtung spaeter in `docs/measurement_logs/` nachziehen

## Schnittstellen/Abhaengigkeiten

- verweist auf `../../preparation/bench_setup/`, `../../tests/safety/`, `../../tests/security/` und `../../SAFETY_FRAMEWORK.md`
