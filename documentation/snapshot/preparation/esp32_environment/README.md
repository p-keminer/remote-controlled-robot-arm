# preparation/esp32_environment

## Zweck

Dieser Ordner haelt Board-, Toolchain- und Stackgrundlagen fuer die ESP32-Seite fest.

## Aktueller Stand

Das Projekt nutzt ESP32-S3-Boards mit dem Custom-FQBN:

```text
esp32:esp32:robotic_arm_s3n16r8
```

Dieser Stand ist verbindlich fuer die aktive Prototyp-Firmware.
Die Boards laufen als Controller, Receiver und Bridge im getrennten Steuer- und Debugpfad.

## Bestaetigte Eckdaten

- Modulbasis: ESP32-S3-WROOM-1-N16R8
- Boardrevision der vorhandenen DevKits dokumentiert
- RGB-LED auf GPIO48 im genutzten Bestand bestaetigt
- Greifer-Eingabe aktuell als Potentiometer auf GPIO1
- WSL wird zusaetzlich fuer ROS, Snapshot-Skripte und Hilfsautomation genutzt

## Regeln

- Das Custom-FQBN bleibt die Repo-Quelle der Wahrheit.
- Lokale Ist-Versionen und Sonderkonfigurationen gehoeren in gitignorierte Dateien.
- Historische generische Boardbezeichnungen duerfen nicht den aktiven Repo-Stand ueberschreiben.

## Verwandte Dokumente

- `../arduino_ide_setup/README.md`
- `../../HELP.md`
- `../../hardware/electronics/PIN_MAPPING_CONFIRMED.md`
