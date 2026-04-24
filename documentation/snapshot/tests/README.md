# tests

## Zweck

Dieser Ordner sammelt nachvollziehbare Nachweise fuer Bench, Integration, Latenz, Safety und Security.

## Aktueller Stand

Die Tests sind nicht mehr nur Struktur:

- der Steuerpfad Controller -> Receiver -> Arduino ist als aktive Kette vorhanden
- der Debugpfad Controller -> Bridge -> MQTT -> Dashboard/ROS ist verifiziert
- dated Nachweise fuer Twin- und Bridge-Validierung liegen in `docs/measurement_logs/`

Weiterhin offen bleiben formale Safety-, Security- und Langzeitnachweise.

## Inhalt

- `bench/` fuer fokussierte Teiltests
- `integration/` fuer Ende-zu-Ende-Pfade
- `latency/` fuer Zeitverhalten
- `safety/` fuer Bewegungs- und Fehlergrenzen
- `security/` fuer Angriffs- und Missbrauchsnachweise

## Regeln

- Debug-Erfolg und Twin-Erfolg werden nicht mit Produktivfreigabe verwechselt.
- dated Mess- und Testnachweise sind die Referenz fuer erledigte Aussagen.

## Schnittstellen/Abhaengigkeiten

- `../docs/measurement_logs/`
- `../security/`
- `../SAFETY_FRAMEWORK.md`
