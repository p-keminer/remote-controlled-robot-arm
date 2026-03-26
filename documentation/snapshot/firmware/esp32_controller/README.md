# firmware/esp32_controller

## Zweck

Dieser Ordner ist fuer die Wearable-Firmware des Controller-ESP32 reserviert.

## Aktueller Stand

Der Teilbereich ist als Bench-Firmware vorhanden.
`esp32_controller.ino` liest drei BNO055 ueber PCA9548A (Mux-Kanaele 0/1/2), erfasst den Flex-Sensor und sendet `ImuPaket v3` per `ESP-NOW` mit Kalibrierungsstatus pro Sensor.
Kalibrierungsoffsets werden persistent im NVS gespeichert und beim Boot geladen. Einzelkalibrierung per Serial (CAL0/CAL1/CAL2).
LED-Debugging mit Ampelsystem (Gruen/Gelb/Rot fuer IMUs, Blau COMMS, Weiss FAULT) und Live-Sensorausfallerkennung.
Naechste Schritte sind UART zum Arduino sowie eine spaetere Protokollerweiterung mit klaren Erweiterungspunkten fuer den Security-Uplift.

## Inhalt

- Sensorinitialisierung fuer drei IMUs ueber PCA9548A-Mux
- ADC-Erfassung fuer den Flex-Sensor mit Live-Plausibilitaetspruefung
- BNO055-Kalibrierungspersistenz im NVS (automatisches Speichern und Laden)
- Einzelkalibrierungsmodus per Serial (CAL0/CAL1/CAL2, RECAL, STOP)
- LED-Debugging: Ampelsystem (Hand/Unterarm/Oberarm) + COMMS + FAULT
- Live-Sensorausfallerkennung mit automatischer Wiederherstellung
- Bench-Vorverarbeitung und Funkuebergabe an den Receiver
- spaetere Referenz-, Mapping- und Security-Erweiterung

## Regeln

- der Controller sendet Zielwerte, aber keine direkte Servoansteuerung
- Sensorzuordnung, Achsinvertierungen und Nullpunktannahmen muessen mit `calibration/` abgestimmt sein
- Kommunikationsdetails gehoeren nicht nur in den Code, sondern auch in `COMMUNICATION_FRAMEWORK.md`
- sobald das Bench-Paket weiterentwickelt wird, muessen `protokoll_version`, `flags` oder andere klare Erweiterungspunkte fuer den spaeteren Security-Uplift mitgedacht werden

## Schnittstellen/Abhaengigkeiten

- liest Regeln aus `../../CALIBRATION_FRAMEWORK.md`
- nutzt die Funkvorgaben aus `../../COMMUNICATION_FRAMEWORK.md`
- liefert Zielwerte an `../esp32_receiver/`
- bezieht lokale Peer- und spaetere Auth-Konfiguration aus gitignorierten Headern
