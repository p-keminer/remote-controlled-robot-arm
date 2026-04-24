# hardware/electronics

## Zweck

Dieser Ordner beschreibt die aktive Elektronik des Prototyps und ordnet historische Entwicklungsstufen ein.

## Aktueller Stand

Aktiv ist kein reiner Breadboard-Pfad mehr, sondern ein Prototyp mit Controller auf Lochraster-/Perfboard.
Die Elektronikbasis umfasst:

- Controller mit drei BNO055 ueber PCA9548A
- Potentiometer als Greifer-Eingabe auf GPIO1
- Receiver mit I2C zum Arduino
- Bridge fuer den verifizierten MQTT-Debugpfad

Historische Breadboard-Planung bleibt als Herkunft erhalten, der aktuelle Zielstand ist jedoch der robuste Prototyp.

## Inhalt

- `SCHEMATIC_CURRENT.md` fasst den aktuellen Verdrahtungsstand zusammen
- `PIN_MAPPING_CONFIRMED.md` haelt die bestaetigte GPIO-Belegung fest
- `FINAL_PERFBOARD_PLAN.md` beschreibt den aktuellen robusteren Controllerstand
- `PROTO_BREADBOARD_PLAN.md` bleibt als historische Zwischenstufe
- `ESP_INTEGRATION_CONCEPT.md` beschreibt die Rollen von Controller, Receiver und Bridge
- `POWER_SUPPLY_CONCEPT.md` behandelt die Strompfadfragen
- `DEBUG_SIGNALS_AND_BUZZER.md` dokumentiert die Statusanzeigen

## Regeln

- `PIN_MAPPING_CONFIRMED.md` und `SCHEMATIC_CURRENT.md` sind die aktiven Referenzen fuer belegte Pins und Verdrahtung.
- Historische Breadboard-Dokumente bleiben sichtbar als Historie markiert.
- Reale lokale Verdrahtungsdetails und Umbauten werden in dated Nachweisen unter `docs/` ergaenzt.

## Schnittstellen/Abhaengigkeiten

- `../../firmware/`
- `../../preparation/`
- `../../docs/`
