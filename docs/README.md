# docs

## Zweck

Dieser Ordner enthaelt die manuell gepflegte Arbeits- und Nachweisdokumentation des Projekts.

## Aktueller Stand

Neben den fruehen Stock- und Bench-Nachweisen liegen jetzt auch dated Dokumente fuer den aktuellen Prototyp- und Twin-Stand vor.
Historische Logs bleiben erhalten; neue dated Nachweise dokumentieren Perfboard, Assembly-Rework, aktuelles Wiring sowie die verifizierte Debug-Bridge und den gemappten Digital Twin.

## Inhalt

- `HARDWARE_INVENTORY.md` — aktueller Material- und Prototypstand
- `ASSEMBLY_LOG_2026-03-24.md` — historischer Stock-Aufbau
- `ASSEMBLY_LOG_2026-04-24.md` — aktueller Prototyp- und Perfboard-Aufbau
- `WIRING_LOG_2026-04-02.md` — historischer Bench-Wiring-Stand
- `WIRING_LOG_2026-04-24.md` — aktueller Prototyp-Wiring-Stand
- `SENSOR_MOUNTING_RECORD.md` — aktuelle Sensor- und Greifer-Eingabe-Zuordnung
- `measurement_logs/` — dated Mess- und Verifikationsprotokolle
- `photos/` — Bild- und README-Medien
- `templates/` — Vorlagen fuer weitere Nachweise

## Regeln

- dated Logs bleiben als Nachweis erhalten und werden nicht still ueberschrieben
- neue reale Erkenntnisse werden als eigene dated Dokumente erfasst
- reproduzierbare Erkenntnisse aus `docs/` muessen bei Bedarf in Leitdokumente oder Bereichsdoku uebernommen werden

## Schnittstellen / Abhaengigkeiten

- wird im generierten Snapshot unter `documentation/` gesammelt
- liefert Nachweise fuer `hardware/`, `calibration/`, `tests/`, `dashboard/` und `ros2/`
- liefert Template-Grundlagen fuer `preparation/` und `hardware/`
