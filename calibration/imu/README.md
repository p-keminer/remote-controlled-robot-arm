# calibration/imu

## Zweck

Dieser Ordner ist fuer IMU-bezogene Referenzdaten und Kalibrierregeln reserviert.

## Aktueller Stand

Die Struktur ist vorbereitet, konkrete Sensorzuordnungen und Achskonventionen folgen spaeter.

## Inhalt

- Zuordnung von IMUs zu Oberarm, Unterarm und Hand
- Achsorientierung und eventuelle Invertierungen
- Nullpunkt- oder Referenzkalibrierung fuer Start und Re-Kalibrierung

## Regeln

- jede Sensorposition klar benennen
- Abweichungen in der Montage nicht implizit behandeln, sondern dokumentieren

## Schnittstellen/Abhaengigkeiten

- folgt `../README.md` und `../../CALIBRATION_FRAMEWORK.md`
- beeinflusst `../../firmware/esp32_controller/` und `../../hardware/mechanik/`
