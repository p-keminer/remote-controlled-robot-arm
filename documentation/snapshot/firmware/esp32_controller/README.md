# firmware/esp32_controller

## Zweck

Dieser Ordner ist fuer die Wearable-Firmware des Controller-ESP32 reserviert.

## Aktueller Stand

Der Teilbereich ist als Bench-Firmware vorhanden und bench-validiert.
`esp32_controller.ino` liest drei BNO055 ueber PCA9548A (Mux-Kanaele 0/1/2), erfasst den Flex-Sensor und sendet `ImuPaket v4` per `ESP-NOW` an zwei Peers: Receiver (Steuerpfad) und Bridge (Debug-Pfad).
ImuPaket v4 enthaelt ein `flags`-Bitfeld (Bit 0 = Notaus) und ist 59 Bytes gross.
Notaus-Toggle-Button auf GPIO21: Jeder Tastendruck toggelt den Notaus-Zustand. Das Notaus-Flag wird per ImuPaket an Receiver und Bridge propagiert.
Kalibrierungsoffsets werden persistent im NVS gespeichert und beim Boot geladen. Einzelkalibrierung per Serial (CAL0/CAL1/CAL2).
LED-Schema invertiert: aus = OK, an = Problem. RGB auf GPIO48: orange blinkend bei Notaus, rot blinkend bei Fehler, aus wenn OK.
Naechste Schritte sind UART zum Arduino sowie eine spaetere Protokollerweiterung mit klaren Erweiterungspunkten fuer den Security-Uplift.

## Inhalt

- Sensorinitialisierung fuer drei IMUs ueber PCA9548A-Mux
- ADC-Erfassung fuer den Flex-Sensor mit Live-Plausibilitaetspruefung
- BNO055-Kalibrierungspersistenz im NVS (automatisches Speichern und Laden)
- Einzelkalibrierungsmodus per Serial (CAL0/CAL1/CAL2, RECAL, STOP)
- Notaus-Toggle-Button auf GPIO21 mit Entprellung (50ms), Zustandswechsel per Serial geloggt
- ImuPaket v4 mit flags-Bitfeld (Bit 0 = Notaus), propagiert an Receiver und Bridge
- LED-Debugging invertiert (aus=OK, an=Problem): GPIO4 Gruen(S2), GPIO5 Gelb(S1), GPIO6 Rot(S0), GPIO7 Blau(COMMS), GPIO10 Weiss(FAULT), GPIO48 RGB(Notaus orange/Fehler rot)
- Live-Sensorausfallerkennung mit automatischer Wiederherstellung
- Multi-Peer ESP-NOW: sendet an Receiver und Bridge gleichzeitig (BRIDGE_AKTIV Flag)
- Bench-Vorverarbeitung und Funkuebergabe an Receiver und Bridge
- spaetere Referenz-, Mapping- und Security-Erweiterung

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
