# Servo- und Transportentscheidungen

Dieser Pfad bleibt erhalten, weil aktive und archivierte Quellkommentare auf
die nummerierten Abschnitte verweisen. Kanonische Gesamtinformationen stehen in
[docs/HARDWARE.md](../docs/HARDWARE.md),
[docs/PROTOCOL.md](../docs/PROTOCOL.md) und [SAFETY.md](../SAFETY.md).

## 1. Historischer UART-Pfad

Der erste Receiver-Arduino-Pfad nutzte ESP32 GPIO15 als UART-TX und Arduino D2
als SoftwareSerial-RX bei 9600 Baud. Er ist vollstaendig unter
`firmware/archive/uart/` archiviert und kein aktiver Buildpfad.

## 2. Historisches Listen-/Servo-Zeitfenster

SoftwareSerial-PCINT und Timer1-Servo-PWM verursachten auf dem ATmega328P
sichtbaren Jitter. Der archivierte Sketch trennte deshalb ein 100-ms-Listenfenster
von Servo-PWM-Phasen. Diese Komplexitaet war ein Grund fuer die I2C-Migration.

## 3. Servo-Attach und Pinbelegung

Der aktive I2C-Sketch laesst alle Servos dauerhaft attached. Die bestaetigten
Pins sind:

| Achse | Pin |
|---|---:|
| Basis | D9 |
| Schulter | D11 |
| Ellbogen | D3 |
| Handgelenk | D5 |
| Greifer | D6 |

## 4. Mechanische Servolimits

| Achse | Minimum | Maximum |
|---|---:|---:|
| Basis | 12 | 139 |
| Schulter | 35 | 142 |
| Ellbogen | 80 | 175 |
| Handgelenk | 5 | 177 |
| Greifer | 32 | 126 |

Diese Werte sind empirische Betriebsgrenzen des aufgebauten Arms, keine
zertifizierten Schutzgrenzen. Sie duerfen nur nach einer erneuten physischen
Messung geaendert werden. Die Schultergrenze ist bewusst enger als der
theoretisch moegliche Weg.

## 5. Neutral- und Startwerte

Bytewert 128 wird pro Achse auf den Mittelpunkt des jeweiligen Limitbereichs
abgebildet. Die tatsaechlichen ganzzahligen Zielwerte sind daher:

```text
Basis 75, Schulter 88, Ellbogen 127, Handgelenk 91, Greifer 79
```

Pauschale 90 Grad sind nicht der aktive Neutralstand. Neutral, Timeout und
Software-ESTOP verwenden dieselben Mittelpunkte und fahren sie ueber die
Slew-Begrenzung an; sie trennen keine Leistung.

## 6. Sweep und Integergrenzen

Der Sweep arbeitet im Bytebereich um `SWEEP_NEUTRAL=128`. Sein Offset muss
`int16_t` sein. Ein `int8_t`-Offset kann an den Grenzen ueberlaufen und einen
Positionssprung erzeugen. Der aktive I2C-Sweep clampt deshalb bei +/-127.

## 9. Slew-Rate-Limiter

Der aktive Arduino-Sketch aktualisiert mit 50 Hz und begrenzt jede Achse auf
`MAX_SCHRITT=3` Grad pro Loop. Das entspricht rechnerisch maximal etwa
150 Grad/s. Der Receiver besitzt zusaetzliche Byte-Slew-Limits; die Arduino-
Grenze bleibt die letzte Servoebene.

## 11. Archivierter UART-Zyklus

Der UART-Sketch wechselte zwischen SoftwareSerial und Servo-PWM. Er bleibt nur
als Entwicklungsnachweis erhalten und darf nicht mit der aktiven I2C-Firmware
verwechselt werden.

## 12. Aktiver I2C-Test-Sweep

`firmware/esp32_receiver/test_sweep_i2c.h` erzeugt den eingecheckten
Receiver-Default. Er sendet alle 20 ms ein 11-Byte-Frame an Slave `0x42` und
testet die Servo-Kette ohne Live-Mapping. Die Datei bleibt erforderlich, solange
`RECEIVER_MODE_TEST_SWEEP=1` der Default ist.

## 13. I2C-Migration und ISR-Regel

Der aktive Pfad nutzt ESP32 GPIO13/14 zu Arduino A4/A5. Der Arduino-Empfangs-
Callback kopiert im Interruptkontext ausschliesslich die 11 Rohbytes in einen
Puffer. Startbytes, Version, XOR-Pruefsumme, Flags und `map()` werden erst im
normalen `loop()` verarbeitet. Rechenarbeit im ISR stoerte zuvor Timer1 und
fuehrte zu Servo-Zucken.

Das I2C-Spannungsniveau muss vor Betrieb gemaess [SAFETY.md](../SAFETY.md)
verifiziert werden.
