# calibration/flex_sensor

## Zweck

Dieser Ordner ist fuer die Kalibrierung und das Mapping des Flex-Sensors vorgesehen.

## Aktueller Stand

Der Bereich enthaelt bereits reale Rohmessungen unter `MESSWERTE.md`.
Das Greifermapping bleibt vorlaeufig und muss nach Montage am Handschuh bzw. Wearable neu kalibriert werden.

## Inhalt

- ADC-Referenzbereiche fuer gestreckten und gebeugten Finger
- Totzonen, Glattungsannahmen und Mappingbereiche
- Hinweise auf Abhaengigkeit von Handschuh, Montage und Sensoralterung
- reale Rohmessungen und abgeleitete Prozentformel

## Regeln

- Rohmessungen und abgeleitete Mappinggrenzen getrennt dokumentieren
- jede Aenderung am Greifermapping nachvollziehbar begruenden

## Schnittstellen/Abhaengigkeiten

- richtet sich nach `../README.md` und `../../CALIBRATION_FRAMEWORK.md`
- beeinflusst `../../firmware/esp32_controller/` und spaeter `../../firmware/arduino_arm/`
