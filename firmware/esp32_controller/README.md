# firmware/esp32_controller

## Zweck

Dieser Ordner ist fuer die Wearable-Firmware des Controller-ESP32 reserviert.

## Aktueller Stand

Der Teilbereich ist dokumentiert, aber noch nicht implementiert.
Die Controller-Seite ist als Sensor- und Mappinginstanz eingegrenzt.

## Inhalt

- spaetere Sensorinitialisierung fuer IMUs und Multiplexer
- ADC-Erfassung fuer den Flex-Sensor
- Vorverarbeitung, Referenzbezug und Zielwerterzeugung
- Funkuebergabe an den Receiver

## Regeln

- der Controller sendet Zielwerte, aber keine direkte Servoansteuerung
- Sensorzuordnung, Achsinvertierungen und Nullpunktannahmen muessen mit `calibration/` abgestimmt sein
- Kommunikationsdetails gehoeren nicht nur in den Code, sondern auch in `COMMUNICATION_FRAMEWORK.md`

## Schnittstellen/Abhaengigkeiten

- liest Regeln aus `../../CALIBRATION_FRAMEWORK.md`
- nutzt die Funkvorgaben aus `../../COMMUNICATION_FRAMEWORK.md`
- liefert Zielwerte an `../esp32_receiver/`
