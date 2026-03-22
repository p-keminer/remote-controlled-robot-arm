# firmware/esp32_controller

## Zweck

Dieser Ordner ist fuer die Wearable-Firmware des Controller-ESP32 reserviert.

## Aktueller Stand

Der Teilbereich ist als Bench-Firmware vorhanden.
`esp32_controller.ino` liest BNO055 ueber PCA9548A, erfasst den Flex-Sensor und sendet `ImuPaket v1` per `ESP-NOW`; Mapping, Session-/Auth-Haertung und Debugsignale sind noch offen.

## Inhalt

- Sensorinitialisierung fuer IMUs und Multiplexer
- ADC-Erfassung fuer den Flex-Sensor
- Bench-Vorverarbeitung und Funkuebergabe an den Receiver
- spaetere Referenz-, Mapping- und Security-Erweiterung

## Regeln

- der Controller sendet Zielwerte, aber keine direkte Servoansteuerung
- Sensorzuordnung, Achsinvertierungen und Nullpunktannahmen muessen mit `calibration/` abgestimmt sein
- Kommunikationsdetails gehoeren nicht nur in den Code, sondern auch in `COMMUNICATION_FRAMEWORK.md`

## Schnittstellen/Abhaengigkeiten

- liest Regeln aus `../../CALIBRATION_FRAMEWORK.md`
- nutzt die Funkvorgaben aus `../../COMMUNICATION_FRAMEWORK.md`
- liefert Zielwerte an `../esp32_receiver/`
- bezieht lokale Peer- und spaetere Auth-Konfiguration aus gitignorierten Headern
