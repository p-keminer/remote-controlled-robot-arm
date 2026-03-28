# firmware/esp32_receiver

## Zweck

Dieser Ordner ist fuer die Receiver-Firmware des zweiten ESP32 reserviert.

## Aktueller Stand

Der Bereich ist als Bench-Firmware vorhanden und bench-validiert.
`esp32_receiver.ino` empfaengt `ESP-NOW`-Pakete auf WiFi-Kanal 1, prueft Paketgroesse, Protokollversion, XOR-Pruefsumme und Frische.
Der Receiver ist der kritische Steuerpfad zum Arduino und funktioniert unabhaengig von der Bridge.
LED-Schema invertiert: aus = OK, blinken = Problem. RGB auf GPIO48 als FAULT.
UART, Safety-Reaktionen und Security-Uplift sind noch offen.
Der serielle Minimalrahmen fuer die naechste Ausbaustufe ist bereits in `../UART_FRAME_V1.md` dokumentiert.

## Inhalt

- ESP-NOW-Empfangslogik auf WiFi-Kanal 1 (koexistent mit Bridge-WiFi)
- Paketvalidierung und Frischepruefung (ImuPaket v3)
- LED-Debugging invertiert: GPIO4 Gruen(UART), GPIO5 Blau(ESP-NOW), GPIO48 RGB(FAULT)
- UART-Weitergabe an das Adeept-Arduino-Board (noch offen)
- Fehlerreaktion bei ungueltigen oder fehlenden Daten

## Regeln

- der Receiver enthaelt keine direkte Servo-PWM
- Board FQBN: `esp32:esp32:robotic_arm_s3n16r8` — **nie** das generische `esp32s3` verwenden (siehe `../../GLOBAL_RULES.md`)
- unvalidierte Pakete duerfen nicht an den Arduino weitergereicht werden
- Timeout- und Watchdog-Annahmen muessen mit `SAFETY_FRAMEWORK.md` konsistent sein
- das erste UART-Format bleibt bewusst fest, klein und binarisch; Erweiterungen duerfen nur kontrolliert ueber Versions- und Flagfelder erfolgen

## Schnittstellen/Abhaengigkeiten

- empfaengt Zielwerte aus `../esp32_controller/`
- uebergibt serielle Frames an `../arduino_arm/`
- orientiert sich an `../../COMMUNICATION_FRAMEWORK.md` und `../../SAFETY_FRAMEWORK.md`
