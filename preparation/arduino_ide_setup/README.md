# preparation/arduino_ide_setup

## Zweck

Dieser Ordner beschreibt den bevorzugten Alltagsworkflow fuer Build, Upload und serielle Diagnose.

## Verbindlicher Boardpfad

Das Repository fuehrt fuer die ESP32-S3-Boards das Custom-FQBN:

```text
esp32:esp32:robotic_arm_s3n16r8
```

Das ist der Standard fuer den Projektalltag.
Generische `ESP32S3 Dev Module`-Auswahlen sind nur historische oder lokale Ausweichpfade.

## Aktueller Stand

- Arduino IDE ist weiterhin der praktische Hauptpfad fuer Flashen und serielle Diagnose.
- Der Controller wird auf dem aktuellen Prototyp ueber diesen Pfad gepflegt.
- Die serielle Greifer-Diagnose und Kalibrierung laufen ueber denselben Workflow.
- Der offizielle Adeept-Stockpfad mit `Arduino Uno` bleibt fuer das originale Armboard dokumentiert.

## Regeln

- Board-, Core- und lokale Besonderheiten muessen zum Custom-FQBN passen.
- Reale Zugangsdaten oder lokale Port-Notizen gehoeren nicht in committed Doku.
- Stock-Arduino und Projekt-ESP32 werden in der Doku klar getrennt gehalten.

## Verwandte Dokumente

- `../esp32_environment/README.md`
- `../../HELP.md`
- `../../AGENTS.md`
