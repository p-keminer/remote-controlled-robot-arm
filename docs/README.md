# docs

## Zweck

Dieser Ordner enthaelt die manuell gepflegte Arbeits- und Nachweisdokumentation des Projekts.

## Aktueller Stand

Erste reale Nachweisdokumente sind angelegt (Stand 2026-04-02): Hardware-Inventar, Aufbauprotokoll, I2C-Bench-Test, Power-On-Checkliste, Sensormontage und Verkabelungsprotokoll.
Sitzungsnotizen, Messprotokolle, Fotodokumentation und dokumentationsnahe Vorlagen werden hier gesammelt und bewusst nicht generiert.

## Inhalt

- `HARDWARE_INVENTORY.md` — Soll/Ist-Bestandsliste aller Komponenten
- `ASSEMBLY_LOG_2026-03-24.md` — Aufbauprotokoll Stock-Montage
- `BENCH_TEST_I2C_CHAIN_2026-04-02.md` — Bench-Testprotokoll I2C-Kette
- `FIRST_POWER_ON_CHECKLIST.md` — Erstinbetriebnahme-Checkliste aller Geraete
- `SENSOR_MOUNTING_RECORD.md` — Sensorpositionen und Mux-Zuordnung
- `WIRING_LOG_2026-04-02.md` — Verkabelungsprotokoll mit allen Verbindungen
- `session_notes/` fuer Arbeitsnotizen und Zwischenstaende
- `measurement_logs/` fuer Messreihen, Beobachtungen und Versuchsergebnisse
- `photos/` fuer Bildmaterial von Aufbau, Referenzposen und Verkabelung
- `templates/` fuer Projektvorlagen (leer, als Grundlage fuer weitere Eintraege)

## Regeln

- dieser Ordner ist Quellmaterial und wird manuell gepflegt
- Rohnotizen, Messwerte und Fotos sollen so abgelegt werden, dass spaetere Entscheidungen nachvollziehbar bleiben
- reproduzierbare Erkenntnisse aus `docs/` muessen bei Bedarf in die Leitdokumente oder Teilbereichsdokus uebernommen werden
- Templates duerfen keinen erfundenen Realstand vortaeuschen, sondern nur die spaetere Erfassung vorbereiten

## Schnittstellen/Abhaengigkeiten

- wird im generierten Snapshot unter `documentation/` gesammelt
- liefert Nachweise fuer `hardware/`, `calibration/`, `tests/` und Sicherheitsentscheidungen
- liefert Template-Grundlagen fuer `preparation/` und `hardware/`
