# hardware/verkabelung

## Zweck

Dieser Ordner beschreibt die reale Verdrahtung des Prototyps.

## Aktueller Stand

Die Verkabelung ist nicht mehr nur hypothetisch:

- Controller mit IMUs, Mux und Potentiometer
- Receiver mit I2C zum Arduino
- Bridge als separater Debugpfad

Historische Verweise auf fruehere Greifer-Hardware bleiben nur als Herkunft relevant.

## Regeln

- reale Verkabelung wird klar von Hersteller- und Schaltplandoku getrennt
- dated Wiring-Logs in `docs/` sind die Nachweisquelle fuer Umbauten und Reworks

## Schnittstellen/Abhaengigkeiten

- `../electronics/SCHEMATIC_CURRENT.md`
- `../../docs/WIRING_LOG_2026-04-24.md`
