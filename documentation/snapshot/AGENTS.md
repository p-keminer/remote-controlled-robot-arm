# Arbeitsregeln fuer Entwicklungsagenten

## Projektgrenze

Das Repository enthaelt zwei aktive, getrennte Pfade:

- Steuerpfad: Controller -> ESP-NOW -> Receiver -> I2C -> Arduino -> Servos
- Debug-/Twin-Pfad: Controller -> Bridge -> MQTT -> Dashboard/MCP/ROS 2

Der Debug-/Twin-Stack ist kein Altcode. Historische Sketches liegen
ausschliesslich unter `firmware/archive/` und sind keine aktiven Buildziele.

## Vor Aenderungen lesen

- immer: `README.md`, `ROADMAP.md` und diese Datei
- Firmware/Kommunikation: `firmware/README.md`, `docs/PROTOCOL.md`, `docs/BUILD.md`
- Hardware/Kalibrierung: `docs/HARDWARE.md`, `calibration/README.md`
- reale Bewegung: `SAFETY.md` und `docs/TESTING.md`
- Netzwerk, MQTT, MCP oder OTA: `SECURITY.md`, `dashboard/README.md`
- ROS/Twin: `ros2/README.md` und `docs/PROTOCOL.md`

## Verbindliche Regeln

- Den vom Nutzer freigegebenen Umfang nicht eigenstaendig erweitern.
- Bei reiner Dokumentationsarbeit keinerlei Code, Konfiguration, Skripte,
  Medien oder Vendor-Dateien aendern.
- Neue oder geaenderte projekt-eigene Variablen, Struct-Felder, Enums,
  Konstanten und Kommentare auf Deutsch verfassen. Externe APIs und
  Bibliothekstypen bleiben unveraendert; Funktionsnamen innerhalb eines Moduls
  konsistent halten. Bestehenden Code nicht allein dafuer umbenennen.
- Den getesteten Funk-/I2C-Pfad nicht ohne neue Hardwarevalidierung veraendern.
- Realarm- und Twin-Mapping nicht als identisch bezeichnen, solange die
  Implementierungen voneinander abweichen.
- GPIO21 nicht als fail-safe Hardware-Notaus darstellen.
- Die I2C-Pruefung als XOR-Pruefsumme bezeichnen, nicht als echtes CRC-8.
- Nur bestaetigte Tests als bestanden markieren; Compilechecks sind keine
  Hardwarefreigabe.
- Reale MACs, Zugangsdaten, Schluessel, Identitaeten und absolute lokale Pfade
  gehoeren ausschliesslich in gitignorierte `*.local.*`-Dateien.
- Herstellerarchive und entpackte Vendor-Baeume werden nicht versioniert.
  `official_downloads/` enthaelt nur Bezugsquellen und Herkunftshinweise.
- Vor Datei-Loeschungen oder einer Git-History-Umschreibung immer eine explizite
  Freigabe mit konkreter Zielliste einholen.

## Dokumentationspflege

- Kanonische Fakten nur an einer Stelle pflegen und von anderen Dokumenten aus
  verlinken.
- Nach Umbenennungen alle Markdown-Links und relevanten Pfadverweise pruefen.
- Fotos, GIFs und Dashboard-Screenshots bleiben unveraendert, sofern der Nutzer
  nicht ausdruecklich etwas anderes freigibt.
- Der bestehende Sammelstand wird nach Dokumentationsaenderungen aus dem
  Repository-Root CRLF-sicher erneuert:

```bash
tr -d '\r' < scripts/update_docs.sh \
  | sed 's|^project_root=.*$|project_root="$PWD"|' \
  | TZ=Europe/Berlin bash -s
```

## Verifikation

Mindestens passend zum Aenderungsumfang ausfuehren:

```bash
python -B tests/latency/controller_hotloop_guard.py
python -B tests/latency/receiver_live_i2c_guard.py
tr -d '\r' < scripts/secret_scan.sh | bash -s -- --tracked
```

Zusaetzlich interne Markdown-Links, referenzierte lokale Pfade und
`git diff --check` pruefen. Build- und Hardwareergebnisse stehen in
`docs/TESTING.md`.
