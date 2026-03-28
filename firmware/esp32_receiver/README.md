# firmware/esp32_receiver

## Zweck

Dieser Ordner ist fuer die Receiver-Firmware des zweiten ESP32 reserviert.

## Aktueller Stand

Der Bereich ist als Bench-Firmware vorhanden und bench-validiert.
`esp32_receiver.ino` empfaengt `ImuPaket v4` per `ESP-NOW`, prueft Paketgroesse (59 Bytes), Protokollversion, XOR-Pruefsumme und Frische.
Der Receiver ist der kritische Steuerpfad zum Arduino und funktioniert unabhaengig von der Bridge.
Notaus-Flag (Bit 0 im flags-Feld) wird vom Controller empfangen und per RGB-LED angezeigt (orange blinkend).
LED-Schema invertiert: aus = OK, an = Problem. RGB auf GPIO48: orange bei Notaus, rot bei Fehler, aus wenn OK.
UART, Safety-Reaktionen und Security-Uplift sind noch offen.
Der serielle Minimalrahmen fuer die naechste Ausbaustufe ist bereits in `../UART_FRAME_V1.md` dokumentiert.

## Inhalt

- ESP-NOW-Empfangslogik auf WiFi-Kanal 1 (koexistent mit Bridge-WiFi)
- Paketvalidierung und Frischepruefung (ImuPaket v4)
- Notaus-Flag-Empfang mit RGB-Anzeige (orange blinkend bei Notaus)
- LED-Debugging invertiert: GPIO4 Gruen(UART), GPIO5 Blau(ESP-NOW), GPIO48 RGB(FAULT)
- UART-Weitergabe an das Adeept-Arduino-Board (noch offen)
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
