<a id="top"></a>

<div align="center">

[![Deutsch](https://img.shields.io/badge/🇩🇪_Deutsch-24292f?style=for-the-badge)](#deutsch)
[![English](https://img.shields.io/badge/🇬🇧_English-24292f?style=for-the-badge)](#english)

</div>

---

<a id="deutsch"></a>

<a id="kommunikationsprotokolle"></a>

# Kommunikationsprotokolle

Dieses Dokument trennt den bewegungsrelevanten Steuerpfad vom Debug-/Twin-Pfad und beschreibt den aktuell implementierten Stand. Checksummen erkennen einfache Uebertragungsfehler; sie sind keine Authentisierung und keine Security-Grenze.

<a id="de-pfade-verantwortungen"></a>

## Pfade und Verantwortungen

```text
Steuerpfad:
3x BNO055 + Potentiometer
  -> Controller
  -> ESP-NOW
  -> Receiver
  -> I2C
  -> Arduino
  -> 5 Servos

Debug-/Twin-Pfad:
Controller
  -> zweiter ESP-NOW-Peer
  -> Bridge
  -> WiFi / MQTT
  -> Dashboard, MCP und ROS 2
```

Die Bridge ist ein aktiver, separat betreibbarer Debugbestandteil. Sie liegt nicht in Reihe mit dem Steuerpfad; ihr Ausfall darf die Receiver-/Arduino-Kette nicht unterbrechen. Ob der Controller den zweiten Peer sendet, bestimmt die lokale Einstellung `BRIDGE_AKTIV`.

<a id="de-esp-now-imupaket-v4"></a>

## ESP-NOW: `ImuPaket v4`

Controller, Receiver und Bridge verwenden dieselbe gepackte Struktur:

| Feld | Typ | Bedeutung |
| --- | --- | --- |
| `zaehler` | `uint32_t` | monotoner Paketzaehler innerhalb einer Controller-Laufzeit |
| `sensoren[3]` | je 3 `float` | `heading`, `roll`, `pitch` |
| `kalib[3]` | je 4 `uint8_t` | `sys`, `gyro`, `accel`, `mag` |
| `flex_prozent` | `float` | aktueller Potentiometer-Greiferwert; MQTT-Schluessel bleibt kompatibel `f` |
| `flags` | `uint8_t` | Bit 0: Software-Notaus; Bits 1-7 reserviert |
| `protokoll_version` | `uint8_t` | Wert `4` |
| `pruefsumme` | `uint8_t` | XOR ueber alle vorherigen Paketbytes |

Segmentzuordnung: `S0 = Hand/Wrist`, `S1 = Unterarm`, `S2 = Oberarm`. Der Controller zielt nominell auf ein Sendeintervall von 5 ms und sendet per Unicast auf WiFi-Kanal 1. Receiver und Controller setzen Kanal 1 explizit; der WiFi-Access-Point der Bridge muss deshalb ebenfalls auf Kanal 1 liegen.

Der aktuelle Funkpfad ist nicht kryptografisch gehaertet:

- die Controller-Peers verwenden `encrypt=false`;
- der Receiver prueft Laenge, Protokollversion, XOR-Pruefsumme und einen steigenden Zaehler, aber nicht die Quell-MAC aus dem Empfangs-Callback;
- nach 2 s Link-Timeout setzt der Receiver seine Zaehlerannahme zurueck;
- die Bridge prueft zusaetzlich die lokal konfigurierte Controller-MAC;
- `session_id`, Authentisierungstag und belastbarer Replay-Schutz sind nicht implementiert.

Die Implementierungen liegen im [Controller](../firmware/esp32_controller/esp32_controller.ino), [Receiver](../firmware/esp32_receiver/esp32_receiver.ino) und in der [Bridge](../firmware/esp32_bridge/esp32_bridge.ino).

<a id="de-receiver-modi-frische"></a>

## Receiver-Modi und Frische

Der committed und gebaute Default lautet:

```text
RECEIVER_MODE_TEST_SWEEP=1
RECEIVER_MODE_LIVE_I2C=0
RECEIVER_LIVE_ENABLE_BASIS=0
RECEIVER_LIVE_ENABLE_SHOULDER=0
RECEIVER_LIVE_ENABLE_ELBOW=0
RECEIVER_LIVE_ENABLE_WRIST=0
RECEIVER_LIVE_ENABLE_GRIPPER=0
RECEIVER_I2C_SEND_ENABLED=1
```

Der real getestete Livepfad wird nur mit den dokumentierten Compile-Flags aus [`BUILD.md`](BUILD.md#deutsch) aktiviert. Im Live-Modus gilt:

- 20 ms I2C-Intervall, also 50 Hz Zielrate;
- ein ESP-NOW-Paket ist nach 250 ms ohne Aktualisierung stale und erzeugt einen Neutral-Frame;
- Software-Notaus erzeugt einen E-Stop-Frame;
- der ESP-NOW-Callback validiert und puffert nur; I2C-Ausgabe und serielle Diagnose laufen im `loop()`;
- Receiver-Slew pro Frame: Basis/Schulter 2 Byte, Ellbogen/Handgelenk 8 Byte, Greifer 12 Byte;
- deaktivierte Achsen laufen gegen den neutralen Bytewert 128.

Der zusaetzliche 2-s-`EMPFANGS_TIMEOUT` steuert Link-/Fehleranzeige und setzt die Paketzaehlerannahme zurueck. Fuer die Bewegung greift im Live-Modus bereits die 250-ms-Stale-Regel.

<a id="de-i2c-frame-v1"></a>

## I2C Frame V1

Transport: ESP32-Receiver als Master auf `GPIO13`/`GPIO14`, 100 kHz; Arduino als Slave `0x42` auf `A4`/`A5`. Die elektrische Pegelfreigabe wird separat in [`HARDWARE.md`](HARDWARE.md#de-receiver-arduino-i2c) behandelt.

Das Frame ist immer 11 Byte lang:

| Byte | Feld | Bedeutung |
| ---: | --- | --- |
| 0 | `start_a` | `0xA5` |
| 1 | `start_b` | `0x5A` |
| 2 | `protokoll_version` | `1` |
| 3 | `sequenz` | `uint8_t`, Ueberlauf 255 -> 0 |
| 4 | `flags` | Statusbits |
| 5 | `basis_soll` | 0-255 |
| 6 | `schulter_soll` | 0-255 |
| 7 | `ellbogen_soll` | 0-255 |
| 8 | `handgelenk_soll` | 0-255 |
| 9 | `greifer_soll` | 0-255 |
| 10 | `pruefsumme` | XOR ueber Byte 0-9 |

Flags: Bit 0 `VALID`, Bit 1 `NEUTRAL`, Bit 2 `DEGRADED`, Bit 3 `ESTOP`; Bit 4-7 sind reserviert. Feld- und Funktionsnamen im Quellcode heissen aus historischen Gruenden teilweise noch `crc8`. Der implementierte Algorithmus ist jedoch nur eine XOR-Pruefsumme, keine CRC-8.

Der Arduino prueft Startbytes, Version und XOR. `VALID` mappt die fuenf Bytewerte auf die empirischen Servolimits. `NEUTRAL` und `ESTOP` setzen dieselben Mittelwertziele; die Bewegung dorthin bleibt durch den Arduino-Slew von maximal 3 Grad pro 20-ms-Zyklus begrenzt. Die Neutralziele sind `75/88/127/91/79` Grad fuer Basis/Schulter/Ellbogen/Handgelenk/Greifer, nicht pauschal 90 Grad. Nach 1 s ohne gueltiges Frame setzt der Arduino ebenfalls diese Ziele. Das ist ein Software-Rueckfallverhalten und keine elektrische Leistungsabschaltung.

Quelle fuer Sender und Empfaenger: [`i2c_frame.h`](../firmware/esp32_receiver/i2c_frame.h) und [`arduino_arm.ino`](../firmware/arduino_arm/arduino_arm.ino).

<a id="de-mqtt-nicht-ros"></a>

## MQTT ist nicht ROS

Die Bridge publiziert diese MQTT-Topics:

| MQTT-Topic | Inhalt | Verhalten |
| --- | --- | --- |
| `robotarm/imu` | Paketzaehler, drei IMUs, Kalibrierstatus, `f`, Flags, Notaus, Version | pro weitergeleitetem Paket, nicht retained |
| `robotarm/status` | RSSI, Uptime, Paket-/Fehlerzaehler, Rate, Notaus | etwa 1 Hz, retained |
| `robotarm/kalib` | Kalibrierstatus aller drei Sensoren | bei Aenderung, retained |
| `robotarm/ota/log` | OTA-Ereignisse der Bridge | bei Ereignis, nicht retained |

ROS 2 konsumiert standardmaessig nur MQTT `robotarm/imu` und publiziert danach lokale ROS-Schnittstellen:

| ROS-Schnittstelle | Typ/Zweck |
| --- | --- |
| `/joint_states` | gemappte Gelenkstaende fuer RViz/Twin |
| `/robotarm/debug/raw_imu` | optionaler ROS-Debugstream mit MQTT-Rohpayload |
| `/robotarm/debug/mapping` | optionaler ROS-Debugstream mit Mappingdetails |
| `/toggle_arm` | lokaler ROS-Service fuer die Twin-Anzeige `front/back/both` |

Die Pfade `/robotarm/debug/raw_imu` und `/robotarm/debug/mapping` sind ROS-Topics, keine von der ESP32-Bridge publizierten MQTT-Topics. Keiner dieser Debug-/Twin-Pfade speist im aktuellen Stand Bewegungsziele in Receiver oder Arduino ein.

Security-Grenzen und Publish-Rechte stehen in [`SECURITY.md`](../SECURITY.md#deutsch); Nachweise und offene Tests in [`TESTING.md`](TESTING.md#deutsch).

<div align="center">

[![Nach oben](https://img.shields.io/badge/⬆_Nach_oben-24292f?style=for-the-badge)](#top)

</div>

---

<a id="english"></a>

<a id="communication-protocols"></a>

# Communication protocols

This document separates the motion-relevant control path from the debug/twin
path and describes the currently implemented state. Checksums detect simple
transmission errors; they are neither authentication nor a security boundary.

<a id="en-paths-responsibilities"></a>

## Paths and responsibilities

```text
Control path:
3x BNO055 + potentiometer
  -> Controller
  -> ESP-NOW
  -> Receiver
  -> I2C
  -> Arduino
  -> 5 servos

Debug/twin path:
Controller
  -> second ESP-NOW peer
  -> Bridge
  -> WiFi / MQTT
  -> Dashboard, MCP, and ROS 2
```

The bridge is an active debug component that can be operated independently. It
is not in series with the control path, and its failure must not interrupt the
receiver/Arduino chain. The local `BRIDGE_AKTIV` setting determines whether the
controller transmits to the second peer.

<a id="en-esp-now-imupaket-v4"></a>

## ESP-NOW: `ImuPaket v4`

The controller, receiver, and bridge use the same packed structure:

| Field | Type | Meaning |
| --- | --- | --- |
| `zaehler` | `uint32_t` | monotonically increasing packet counter within one controller runtime |
| `sensoren[3]` | three `float` values each | `heading`, `roll`, `pitch` |
| `kalib[3]` | four `uint8_t` values each | `sys`, `gyro`, `accel`, `mag` |
| `flex_prozent` | `float` | current potentiometer-based gripper value; the MQTT key remains `f` for compatibility |
| `flags` | `uint8_t` | bit 0: software E-stop; bits 1-7 reserved |
| `protokoll_version` | `uint8_t` | value `4` |
| `pruefsumme` | `uint8_t` | XOR across all preceding packet bytes |

Segment assignment: `S0 = hand/wrist`, `S1 = forearm`, `S2 = upper arm`. The
controller nominally targets a 5 ms transmission interval and sends unicast on
WiFi channel 1. The receiver and controller set channel 1 explicitly, so the
bridge's WiFi access point must also use channel 1.

The current radio path is not cryptographically hardened:

- the controller peers use `encrypt=false`;
- the receiver validates length, protocol version, XOR checksum, and an
  increasing counter, but it does not validate the source MAC from the receive
  callback;
- after a 2 s link timeout, the receiver resets its counter assumption;
- the bridge additionally validates the locally configured controller MAC;
- `session_id`, an authentication tag, and robust replay protection are not
  implemented.

The implementations are in the [controller](../firmware/esp32_controller/esp32_controller.ino), [receiver](../firmware/esp32_receiver/esp32_receiver.ino), and [bridge](../firmware/esp32_bridge/esp32_bridge.ino).

<a id="en-receiver-modes-freshness"></a>

## Receiver modes and freshness

The committed and compiled default is:

```text
RECEIVER_MODE_TEST_SWEEP=1
RECEIVER_MODE_LIVE_I2C=0
RECEIVER_LIVE_ENABLE_BASIS=0
RECEIVER_LIVE_ENABLE_SHOULDER=0
RECEIVER_LIVE_ENABLE_ELBOW=0
RECEIVER_LIVE_ENABLE_WRIST=0
RECEIVER_LIVE_ENABLE_GRIPPER=0
RECEIVER_I2C_SEND_ENABLED=1
```

The physically tested live path is enabled only with the documented compile
flags from [`BUILD.md`](BUILD.md#english). In live mode:

- the I2C interval is 20 ms, for a 50 Hz target rate;
- an ESP-NOW packet becomes stale after 250 ms without an update and produces a
  neutral frame;
- the software E-stop produces an E-stop frame;
- the ESP-NOW callback only validates and buffers data; I2C output and serial
  diagnostics run in `loop()`;
- receiver slew per frame: 2 bytes for base/shoulder, 8 bytes for elbow/wrist,
  and 12 bytes for the gripper;
- disabled axes move toward the neutral byte value 128.

The additional 2 s `EMPFANGS_TIMEOUT` controls the link/fault indication and
resets the packet-counter assumption. In live mode, motion already uses the
250 ms stale rule.

<a id="en-i2c-frame-v1"></a>

## I2C Frame V1

Transport: ESP32 receiver as master on `GPIO13`/`GPIO14` at 100 kHz; Arduino as
slave `0x42` on `A4`/`A5`. Electrical level approval is handled separately in
[`HARDWARE.md`](HARDWARE.md#en-receiver-arduino-i2c).

The frame is always 11 bytes long:

| Byte | Field | Meaning |
| ---: | --- | --- |
| 0 | `start_a` | `0xA5` |
| 1 | `start_b` | `0x5A` |
| 2 | `protokoll_version` | `1` |
| 3 | `sequenz` | `uint8_t`, wraps from 255 -> 0 |
| 4 | `flags` | status bits |
| 5 | `basis_soll` | 0-255 |
| 6 | `schulter_soll` | 0-255 |
| 7 | `ellbogen_soll` | 0-255 |
| 8 | `handgelenk_soll` | 0-255 |
| 9 | `greifer_soll` | 0-255 |
| 10 | `pruefsumme` | XOR across bytes 0-9 |

Flags: bit 0 `VALID`, bit 1 `NEUTRAL`, bit 2 `DEGRADED`, bit 3 `ESTOP`; bits
4-7 are reserved. Some fields and functions in the source code retain the
historical name `crc8`, but the implemented algorithm is only an XOR checksum,
not CRC-8.

The Arduino validates the start bytes, version, and XOR. `VALID` maps the five
byte values to the empirical servo limits. `NEUTRAL` and `ESTOP` set the same
midpoint targets; motion toward them remains limited by the Arduino slew of at
most 3 degrees per 20 ms cycle. The neutral targets are `75/88/127/91/79`
degrees for base/shoulder/elbow/wrist/gripper, not a uniform 90 degrees. After
1 s without a valid frame, the Arduino also requests these targets. This is
software fallback behavior, not an electrical power cutoff.

The sender and receiver sources are [`i2c_frame.h`](../firmware/esp32_receiver/i2c_frame.h) and [`arduino_arm.ino`](../firmware/arduino_arm/arduino_arm.ino).

<a id="en-mqtt-is-not-ros"></a>

## MQTT is not ROS

The bridge publishes these MQTT topics:

| MQTT topic | Content | Behavior |
| --- | --- | --- |
| `robotarm/imu` | packet counter, three IMUs, calibration status, `f`, flags, E-stop, version | once per forwarded packet, not retained |
| `robotarm/status` | RSSI, uptime, packet/error counters, rate, E-stop | approximately 1 Hz, retained |
| `robotarm/kalib` | calibration status for all three sensors | on change, retained |
| `robotarm/ota/log` | bridge OTA events | on event, not retained |

By default, ROS 2 consumes only the MQTT topic `robotarm/imu` and then publishes
local ROS interfaces:

| ROS interface | Type/purpose |
| --- | --- |
| `/joint_states` | mapped joint states for RViz/twin |
| `/robotarm/debug/raw_imu` | optional ROS debug stream carrying the raw MQTT payload |
| `/robotarm/debug/mapping` | optional ROS debug stream with mapping details |
| `/toggle_arm` | local ROS service for the twin display modes `front/back/both` |

The paths `/robotarm/debug/raw_imu` and `/robotarm/debug/mapping` are ROS topics,
not MQTT topics published by the ESP32 bridge. In the current state, none of
these debug/twin paths injects motion targets into the receiver or Arduino.

Security boundaries and publish permissions are documented in
[`SECURITY.md`](../SECURITY.md#english); evidence and pending tests are documented in
[`TESTING.md`](TESTING.md#english).

<div align="center">

[![Back to top](https://img.shields.io/badge/⬆_Back_to_top-24292f?style=for-the-badge)](#top)

</div>
