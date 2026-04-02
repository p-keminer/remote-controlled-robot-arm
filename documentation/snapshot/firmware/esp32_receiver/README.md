# firmware/esp32_receiver

## Zweck

Dieser Ordner ist fuer die Receiver-Firmware des zweiten ESP32 reserviert.

## Aktueller Stand

Der Bereich ist als Bench-Firmware vorhanden und bench-validiert.
`esp32_receiver.ino` (UART-Variante, archiviert) und `esp32_receiver_i2c.ino` (aktive I2C-Variante) empfangen `ImuPaket v4` per `ESP-NOW`, pruefen Paketgroesse (59 Bytes), Protokollversion, XOR-Pruefsumme und Frische.
Die I2C-Variante sendet Frame V1 (11 Bytes) als I2C-Master (GPIO13 SDA, GPIO14 SCL) an den Arduino-Slave (Adresse 0x42).
Der Receiver ist der kritische Steuerpfad zum Arduino und funktioniert unabhaengig von der Bridge.
Notaus-Flag (Bit 0 im flags-Feld) wird vom Controller empfangen und per RGB-LED angezeigt (orange blinkend).
LED-Schema invertiert: aus = OK, an = Problem. RGB auf GPIO48: orange bei Notaus, rot bei Fehler, aus wenn OK.
Safety-Reaktionen und Security-Uplift sind noch offen.

## Inhalt

- `esp32_receiver_i2c.ino` — aktive I2C-Variante mit Frame V1 Sendung an Arduino
- `esp32_receiver.ino` — Original-UART-Variante (archiviert, nicht aktiv)
- `i2c_frame.h` — I2C-Abstraktionsschicht (Master, Frame V1 senden, 100kHz)
- `test_sweep_i2c.h` — I2C-Sweep-Test (alle 5 Achsen, 50Hz)
- `uart_frame.h`, `test_sweep.h` — UART-Varianten (archiviert)
- ESP-NOW-Empfangslogik auf WiFi-Kanal 1 (koexistent mit Bridge-WiFi)
- Paketvalidierung und Frischepruefung (ImuPaket v4)
- Notaus-Flag-Empfang mit RGB-Anzeige (orange blinkend bei Notaus)
- LED-Debugging invertiert: GPIO4 Gruen(I2C), GPIO5 Blau(LINK), GPIO48 RGB(FAULT)
- I2C-Weitergabe an das Adeept-Arduino-Board (bench-validiert)
- Fehlerreaktion bei ungueltigen oder fehlenden Daten

## Regeln

- der Receiver enthaelt keine direkte Servo-PWM
- unvalidierte Pakete duerfen nicht an den Arduino weitergereicht werden
- Timeout- und Watchdog-Annahmen muessen mit `SAFETY_FRAMEWORK.md` konsistent sein
- das Frame V1 Format bleibt bewusst fest, klein und binaerisch; Erweiterungen duerfen nur kontrolliert ueber Versions- und Flagfelder erfolgen

## Schnittstellen/Abhaengigkeiten

- empfaengt Zielwerte aus `../esp32_controller/`
- uebergibt Frame V1 per I2C an `../arduino_arm/`
- orientiert sich an `../../COMMUNICATION_FRAMEWORK.md` und `../../SAFETY_FRAMEWORK.md`
