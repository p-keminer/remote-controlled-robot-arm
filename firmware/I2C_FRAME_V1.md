# I2C Frame V1

Dieses Dokument bleibt als stabiles Ziel fuer Verweise im aktiven Quellcode
erhalten. Die uebergreifende Protokollbeschreibung steht in
[`docs/PROTOCOL.md`](../docs/PROTOCOL.md).

## Transport

- ESP32-S3 Receiver als Master: SDA GPIO13, SCL GPIO14
- Arduino Uno als Slave: SDA A4, SCL A5, Adresse `0x42`
- Standard-Mode mit 100 kHz
- 11 Byte pro Frame, ohne Padding

Wegen der moeglichen 5-V-Pull-ups auf der Arduino-Seite muss vor dem Betrieb
am ESP32 ein High-Pegel von hoechstens 3,6 V nachgewiesen oder ein geeigneter
bidirektionaler Pegelwandler eingesetzt werden. Details stehen in
[`docs/HARDWARE.md`](../docs/HARDWARE.md) und [`SAFETY.md`](../SAFETY.md).

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

## Flags und Verhalten

| Bit | Name | Bedeutung |
| ---: | --- | --- |
| 0 | `VALID` | Achswerte anwenden |
| 1 | `NEUTRAL` | definierte Mittelziele anfordern |
| 2 | `DEGRADED` | eingeschraenkte Datenqualitaet melden |
| 3 | `ESTOP` | Software-Notaus-Ziel anfordern |
| 4..7 | reserviert | nicht verwenden |

Neutral und Software-Notaus setzen die Servo-Ziele auf
`75/88/127/91/79` Grad und fahren sie mit der vorhandenen Slew-Begrenzung an.
Sie trennen weder Versorgung noch Drehmoment und ersetzen keinen physischen
Not-Halt.

## Implementierungen

- Sender: [`esp32_receiver/i2c_frame.h`](esp32_receiver/i2c_frame.h)
- Empfaenger: [`arduino_arm/arduino_arm.ino`](arduino_arm/arduino_arm.ino)
- Hardware- und Timingbegruendung:
  [`SERVO_UART_DESIGNENTSCHEIDUNGEN.md`](SERVO_UART_DESIGNENTSCHEIDUNGEN.md)

Die Structs auf beiden Seiten muessen bytegenau identisch bleiben. Der
eingecheckte Receiver-Default ist der Test-Sweep; Live-I2C und einzelne Achsen
werden bewusst ueber Compile-Flags freigeschaltet.
