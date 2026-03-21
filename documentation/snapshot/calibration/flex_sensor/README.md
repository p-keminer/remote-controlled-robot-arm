# calibration/flex_sensor

## Zweck

Dieser Ordner ist fuer die Kalibrierung und das Mapping des Flex-Sensors vorgesehen.

## Aktueller Stand

Der Bereich ist vorbereitet, Messwerte und Greifermapping werden spaeter hier dokumentiert.

## Inhalt

- ADC-Referenzbereiche fuer gestreckten und gebeugten Finger
- Totzonen, Glattungsannahmen und Mappingbereiche
- Hinweise auf Abhaengigkeit von Handschuh, Montage und Sensoralterung

## Regeln

- Rohmessungen und abgeleitete Mappinggrenzen getrennt dokumentieren
- jede Aenderung am Greifermapping nachvollziehbar begruenden

## Schnittstellen/Abhaengigkeiten

- richtet sich nach `../README.md` und `../../CALIBRATION_FRAMEWORK.md`
- beeinflusst `../../firmware/esp32_controller/` und spaeter `../../firmware/arduino_arm/`
