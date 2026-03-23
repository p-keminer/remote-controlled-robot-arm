# firmware/esp32_receiver

## Zweck

Dieser Ordner ist fuer die Receiver-Firmware des zweiten ESP32 reserviert.

## Aktueller Stand

Der Bereich ist als Bench-Firmware vorhanden.
`esp32_receiver.ino` empfaengt `ESP-NOW`-Pakete, prueft Paketgroesse, Protokollversion, XOR-Pruefsumme und Frische; UART, Safety-Reaktionen und Security-Uplift sind noch offen.
Der serielle Minimalrahmen fuer die naechste Ausbaustufe ist bereits in `../UART_FRAME_V1.md` dokumentiert, wird aber erst nach drittem IMU und realer Grundkette bench-validiert.

## Inhalt

- ESP-NOW-Empfangslogik fuer den Bench-Pfad
- Paketvalidierung und erste Frischepruefung
- UART-Weitergabe an das Adeept-Arduino-Board
- Fehlerreaktion bei ungueltigen oder fehlenden Daten

## Regeln

- der Receiver enthaelt keine direkte Servo-PWM
- unvalidierte Pakete duerfen nicht an den Arduino weitergereicht werden
- Timeout- und Watchdog-Annahmen muessen mit `SAFETY_FRAMEWORK.md` konsistent sein
- das erste UART-Format bleibt bewusst fest, klein und binarisch; Erweiterungen duerfen nur kontrolliert ueber Versions- und Flagfelder erfolgen

## Schnittstellen/Abhaengigkeiten

- empfaengt Zielwerte aus `../esp32_controller/`
- uebergibt serielle Frames an `../arduino_arm/`
- orientiert sich an `../../COMMUNICATION_FRAMEWORK.md` und `../../SAFETY_FRAMEWORK.md`
