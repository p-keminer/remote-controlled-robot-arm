# firmware/esp32_receiver

## Zweck

Dieser Ordner enthaelt die Receiver-Firmware des steuerrelevanten zweiten ESP32.

## Aktueller Stand

Der Receiver empfaengt den Controller-Funkpfad und uebergibt ihn per I2C an das Arduino-Armboard:

- ESP-NOW-Empfang von `ImuPaket v4`
- Paketvalidierung und Frischepruefung
- I2C-Ausgabe als `Frame V1` an den Arduino-Slave `0x42`
- Notaus- und Statusanzeige ueber LEDs/RGB

Der Receiver ist bewusst vom Debugpfad getrennt und bleibt fuer die Bewegung kritisch.

## Inhalt

- `esp32_receiver_i2c.ino` als aktive Variante
- archivierte UART-Variante
- `i2c_frame.h` fuer den Transport zum Arduino
- Testhilfen fuer I2C und Sweep

## Regeln

- Ungueltige Frames duerfen nicht an den Arduino weitergereicht werden.
- Der Receiver enthaelt keine direkte Servo-PWM.
- Timeout- und Fehlerverhalten bleiben Teil von Safety und duerfen nicht nur implizit im Code existieren.

## Schnittstellen/Abhaengigkeiten

- liest von `../esp32_controller/`
- schreibt an `../arduino_arm/`
- folgt `../../COMMUNICATION_FRAMEWORK.md` und `../../SAFETY_FRAMEWORK.md`
