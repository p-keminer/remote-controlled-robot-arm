# UART Frame V1 (archiviert)

Diese Spezifikation gehoert zum historischen UART-Pfad unter
[`firmware/archive/uart/`](archive/uart/). Der aktive Receiver-Arduino-Pfad
verwendet I2C; siehe [`I2C_FRAME_V1.md`](I2C_FRAME_V1.md) und die kanonische
Beschreibung in [`docs/PROTOCOL.md`](../docs/PROTOCOL.md).

## Transport

- ESP32 Hardware-UART TX: GPIO15, `9600 8N1`
- Arduino SoftwareSerial RX: D2
- Einwegverbindung vom Receiver zum Arduino
- 11 Byte pro Frame, ohne Padding

## Frame

| Byte | Feld | Wert oder Bedeutung |
| ---: | --- | --- |
| 0 | `start_a` | `0xA5` |
| 1 | `start_b` | `0x5A` |
| 2 | `protokoll_version` | `1` |
| 3 | `sequenz` | `uint8_t`, Ueberlauf ist erlaubt |
| 4 | `flags` | Bitfeld |
| 5 | `basis_soll` | `0..255` |
| 6 | `schulter_soll` | `0..255` |
| 7 | `ellbogen_soll` | `0..255` |
| 8 | `handgelenk_soll` | `0..255` |
| 9 | `greifer_soll` | `0..255` |
| 10 | `crc8` | XOR der Bytes 0 bis 9 |

`crc8` ist der historische Feld- und Funktionsname. Implementiert ist eine
einfache XOR-Pruefsumme, kein CRC-8-Verfahren.

Flags: Bit 0 = gueltig, Bit 1 = Neutralziel, Bit 2 = degraded, Bit 3 =
Software-Notaus. Neutral und Software-Notaus setzen nur Servo-Zielwerte; sie
trennen weder Versorgung noch Drehmoment.

Implementierungen:

- [`uart_frame.h`](archive/uart/esp32_receiver_legacy_uart/uart_frame.h)
- [`arduino_arm_uart.ino`](archive/uart/arduino_arm_uart.ino)

Der UART-Pfad ist nur zur Nachvollziehbarkeit archiviert und nicht Teil des
aktuellen Build- oder Flashpfads.
