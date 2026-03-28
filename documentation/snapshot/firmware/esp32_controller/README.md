# firmware/esp32_controller

## Zweck

Dieser Ordner ist fuer die Wearable-Firmware des Controller-ESP32 reserviert.

## Aktueller Stand

Der Teilbereich ist als Bench-Firmware vorhanden und bench-validiert.
`esp32_controller.ino` liest drei BNO055 ueber PCA9548A (Mux-Kanaele 0/1/2), erfasst den Flex-Sensor und sendet `ImuPaket v3` per `ESP-NOW` an zwei Peers: Receiver (Steuerpfad) und Bridge (Debug-Pfad).
Kalibrierungsoffsets werden persistent im NVS gespeichert und beim Boot geladen. Einzelkalibrierung per Serial (CAL0/CAL1/CAL2).
WiFi-Kanal 1 wird per `esp_wifi_set_channel()` gesetzt fuer Koexistenz mit der Bridge (WiFi) und dem Router.
LED-Schema invertiert: aus = OK, blinken = Problem. RGB auf GPIO48 als FAULT (Sensorausfall, Flex-Fehler).
Naechste Schritte sind UART zum Arduino sowie eine spaetere Protokollerweiterung mit klaren Erweiterungspunkten fuer den Security-Uplift.

## Inhalt

- Sensorinitialisierung fuer drei IMUs ueber PCA9548A-Mux
- ADC-Erfassung fuer den Flex-Sensor mit Live-Plausibilitaetspruefung
- BNO055-Kalibrierungspersistenz im NVS (automatisches Speichern und Laden)
- Einzelkalibrierungsmodus per Serial (CAL0/CAL1/CAL2, RECAL, STOP)
- LED-Debugging invertiert (aus=OK, blinken=Problem): GPIO4 Gruen(S2), GPIO5 Gelb(S1), GPIO6 Gelb(S0), GPIO7 Blau(COMMS), GPIO48 RGB(FAULT)
- Live-Sensorausfallerkennung mit automatischer Wiederherstellung
- Multi-Peer ESP-NOW: sendet an Receiver und Bridge gleichzeitig (BRIDGE_AKTIV Flag)
- WiFi-Kanal 1 per esp_wifi_set_channel() fuer Bridge-Koexistenz
- Bench-Vorverarbeitung und Funkuebergabe an Receiver und Bridge
- spaetere Referenz-, Mapping- und Security-Erweiterung

## Board

FQBN: `esp32:esp32:robotic_arm_s3n16r8` — **nie** das generische `esp32s3` verwenden (Reset-Loop durch fehlende CDC). Siehe `../../GLOBAL_RULES.md` Abschnitt "Board- und Flash-Konfiguration".

## Regeln

- der Controller sendet Zielwerte, aber keine direkte Servoansteuerung
- Sensorzuordnung, Achsinvertierungen und Nullpunktannahmen muessen mit `calibration/` abgestimmt sein
- Kommunikationsdetails gehoeren nicht nur in den Code, sondern auch in `COMMUNICATION_FRAMEWORK.md`
- sobald das Bench-Paket weiterentwickelt wird, muessen `protokoll_version`, `flags` oder andere klare Erweiterungspunkte fuer den spaeteren Security-Uplift mitgedacht werden

## Schnittstellen/Abhaengigkeiten

- liest Regeln aus `../../CALIBRATION_FRAMEWORK.md`
- nutzt die Funkvorgaben aus `../../COMMUNICATION_FRAMEWORK.md`
- liefert Zielwerte an `../esp32_receiver/` (Steuerpfad) und `../esp32_bridge/` (Debug-Pfad)
- bezieht lokale Peer- und spaetere Auth-Konfiguration aus gitignorierten Headern
