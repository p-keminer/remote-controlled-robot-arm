<a id="top"></a>

<div align="center">

[![Deutsch](https://img.shields.io/badge/🇩🇪_Deutsch-24292f?style=for-the-badge)](#deutsch)
[![English](https://img.shields.io/badge/🇬🇧_English-24292f?style=for-the-badge)](#english)

</div>

---

<a id="deutsch"></a>

<a id="hardware"></a>

# Hardware

Diese Datei ist die kompakte Hardware-Referenz des aktuellen Prototyps. Sie
beschreibt den real aufgebauten Projektpfad; Herstellerstand und fruehe
Breadboard-/UART-/Flex-Sensor-Staende sind keine aktive Verdrahtung.

Der Arm ist ein funktionsfaehiger Entwicklungsprototyp, aber kein
sicherheitszertifiziertes Produkt. Vor realer Bewegung gelten zusaetzlich die
Einschraenkungen aus [`SAFETY.md`](../SAFETY.md#deutsch).

<a id="de-aktive-komponenten"></a>

## Aktive Komponenten

- Adeept 5-DOF Roboterarm mit ATmega328P-basiertem Driver Board
  (`arduino:avr:uno`)
- fuenf aktive AD002-Servos; der urspruengliche Schulterservo wurde durch den
  damaligen Reserveservo ersetzt, ein weiterer Reserveservo ist nicht vorhanden
- drei ESP32-S3-WROOM-1-N16R8: Controller, Receiver und separate Debug-Bridge;
  ein viertes Board ist Reserve
- Boardrevision v1.0 mit 16 MB Flash, 8 MB Octal-PSRAM und RGB-LED auf `GPIO48`
- drei GY-BNO055-Module an einem PCA9548A-Multiplexer
- 10-kOhm-Potentiometer als aktive Greifer-Eingabe
- zwei Molicel INR-18650-M35A im Adeept-Batteriehalter; zwei weitere Zellen als
  Wechselakkus, Ladegeraet XTAR VC4SL

Fuer die ESP32 gilt die Custom-FQBN
`esp32:esp32:robotic_arm_s3n16r8`. `GPIO35`, `GPIO36` und `GPIO37` sind durch das
Octal-PSRAM belegt und nicht fuer Projekt-I/O verfuegbar.

<a id="de-controller-sensorik"></a>

## Controller und Sensorik

| Signal | Anschluss | Gegenstelle / Funktion |
| --- | --- | --- |
| ADC Greifer | `GPIO1` | Poti-Wischer; 10-kOhm-Pull-down bleibt am Controller |
| I2C SDA | `GPIO8` | PCA9548A SDA |
| I2C SCL | `GPIO9` | PCA9548A SCL |
| IMU-Status S2 | `GPIO4` | blaue LED, Oberarm |
| IMU-Status S1 | `GPIO5` | rote LED, Unterarm |
| IMU-Status S0 | `GPIO6` | weisse LED, Hand/Wrist |
| Kommunikation | `GPIO7` | gruene LED |
| Fehler | `GPIO10` | externe FAULT-LED |
| Software-Stop | `GPIO21` | Taster nach GND, interner Pull-up |
| RGB | `GPIO48` | onboard, Fehler-/Stop-Anzeige |

PCA9548A-Adresse: `0x70`. Alle drei BNO055 verwenden hinter getrennten
Mux-Kanaelen die Adresse `0x29`.

| Mux-Kanal | Datenindex | Koerpersegment |
| --- | --- | --- |
| `0` | `S0` | Hand/Wrist |
| `1` | `S1` | Unterarm |
| `2` | `S2` | Oberarm |

Das Potentiometer ist im bestaetigten 2-Draht-Aufbau angeschlossen: `3.3 V` an
einem aeusseren Pin, Wischer an `GPIO1`; der vorhandene 10-kOhm-Pull-down zieht
den ADC-Knoten nach GND. Die aktiven Kalibrieranker stehen in
[`calibration/README.md`](../calibration/README.md).

<a id="de-receiver-arduino-i2c"></a>

## Receiver, Arduino und I2C

| Signal | ESP32-Receiver | Adeept-/Arduino-Board |
| --- | --- | --- |
| SDA | `GPIO13` | `A4`, OLED-Header Pin 3 |
| SCL | `GPIO14` | `A5`, OLED-Header Pin 4 |
| Masse | `GND` | `GND`, gemeinsamer Bezug |

- ESP32: I2C-Master mit `100 kHz`
- Arduino: I2C-Slave `0x42`
- Transport: festes 11-Byte-Frame V1
- das OLED muss waehrend dieser Verbindung ausgesteckt bleiben, weil es denselben
  Busanschluss belegt
- Receiver-Status: `GPIO4` I2C, `GPIO5` ESP-NOW/LINK, `GPIO48` RGB

Die aktive Definition liegt in
[`i2c_frame.h`](../firmware/esp32_receiver/i2c_frame.h) und
[`arduino_arm.ino`](../firmware/arduino_arm/arduino_arm.ino).

<a id="de-i2c-pegel"></a>

### I2C-Pegel: vor Betrieb verifizieren

Der ESP32-S3 arbeitet mit `3.3 V`, das ATmega328P-Board mit `5 V`. I2C ist
Open-Drain; der HIGH-Pegel wird daher durch die tatsaechlich vorhandenen
Pull-ups bestimmt. Eine funktionierende Bench-Verbindung beweist nicht, dass
am ESP32 ein zulaessiger Pegel anliegt. Die fruehere Annahme, interne
ATmega-Pull-ups seien ohne Pegelanpassung unkritisch, ist keine belastbare
Freigabe.

Vor erneutem Anschluss an einen ESP32:

1. alle Pull-ups auf Driver Board, OLED-Header und Zusatzmodulen identifizieren;
2. SDA und SCL im Leerlauf gegen GND messen;
3. sicherstellen, dass beide Leitungen auf der ESP32-Seite auf `3.3 V`
   hochgezogen werden und dort niemals oberhalb des Datenblattbereichs liegen;
4. bei `5 V` oder unbekannter Beschaltung einen bidirektionalen, fuer
   Standard-Mode-I2C geeigneten Levelshifter einsetzen oder die Pull-ups
   nachweislich auf `3.3 V` legen;
5. erst danach ESP32, Arduino und gemeinsame Masse verbinden.

`5 V` direkt an einem ESP32-S3-GPIO ist nicht freigegeben. Bis Pull-up-Spannung
und Leerlaufpegel dokumentiert sind, bleibt diese elektrische Schnittstelle ein
offener Safety-Punkt.

<a id="de-servoanschluesse-grenzen"></a>

## Servoanschluesse und aktive Grenzen

Die reale Belegung weicht vom Adeept-Stock-Mapping ab. Die aktive Firmware
mappt jeweils den Bytebereich `0..255` auf die gemessenen Min-/Max-Werte.

| Gelenk | PWM-Pin | Stock-Poti | Min | Max | aktiver Start-/Rueckfallwert |
| --- | ---: | ---: | ---: | ---: | ---: |
| Basis | `D9` | `A0` | 12 Grad | 139 Grad | 75 Grad |
| Schulter | `D11` | `A6` | 35 Grad | 142 Grad | 88 Grad |
| Ellbogen | `D3` | `A3` | 80 Grad | 175 Grad | 127 Grad |
| Handgelenk | `D5` | `A2` | 5 Grad | 177 Grad | 91 Grad |
| Greifer | `D6` | `A1` | 32 Grad | 126 Grad | 79 Grad |

Die Werte `75/88/127/91/79` sind die ganzzahligen Mittelpunkte der aktiven
Grenzen. `90 Grad` war ein historischer Montage-/Kalibrierstand, ist aber nicht
die aktuelle Start-, Timeout- oder Software-Stop-Pose. Die Grenzen wurden
empirisch am Prototyp bestimmt; sie sind kein normativer Sicherheitsnachweis.

<a id="de-debug-bridge"></a>

## Debug-Bridge

Die Bridge ist ein eigener Beobachtungsknoten und gehoert nicht zum
Bewegungsweg.

| Signal | Pin |
| --- | --- |
| WiFi-Status | `GPIO4` |
| ESP-NOW-Status | `GPIO5` |
| MQTT-Status | `GPIO7` |
| RGB | `GPIO48` |

Ihr Ausfall darf den Pfad `Controller -> Receiver -> Arduino -> Servos` nicht
veraendern.

<a id="de-stromversorgung"></a>

## Stromversorgung

- Mehrservo-Bewegung wird nicht aus dem USB-Uploadpfad freigegeben.
- Der bestaetigte Prototyppfad nutzt zwei qualifizierte 18650-Zellen im
  Adeept-Batteriehalter; USB dient Upload und Diagnose.
- Servolast und empfindliche Logikversorgung sind getrennt zu planen, brauchen
  fuer die Signale aber einen bewusst hergestellten gemeinsamen Massebezug.
- Verschiedene Versorgungen duerfen nicht unbeabsichtigt ueber USB, Header oder
  Breadboard parallelgeschaltet werden.
- Stromaufnahme, Sicherung, Leitungsquerschnitt, Steckverbindungen und
  Zugentlastung sind vor einem Dauerbetrieb noch nachzuweisen.

<a id="de-primaerquellen"></a>

## Primaerquellen

- [Aktive Controller-Firmware](../firmware/esp32_controller/esp32_controller.ino)
- [Aktive Receiver-Firmware](../firmware/esp32_receiver/esp32_receiver.ino)
- [Aktive Arduino-Firmware](../firmware/arduino_arm/arduino_arm.ino)
- [ESP32-S3 Series Datasheet](https://documentation.espressif.com/esp32_s3_datasheet_en.pdf)
  fuer Versorgungs- und GPIO-Pegelgrenzen
- [Arduino UNO Rev3](https://store.arduino.cc/products/arduino-uno-rev3) fuer
  ATmega328P und `5 V` Betriebsspannung
- [Microchip TWI-Beispiel](https://onlinedocs.microchip.com/oxy/GUID-317042D4-BCCE-4065-BB05-AC4312DBC2C4-en-US-2/GUID-DEDA6A06-3138-45C3-BB95-74E4CF6BC406.html)
  fuer Open-Drain-Bus und erforderliche Pull-ups

<div align="center">

[![Nach oben](https://img.shields.io/badge/⬆_Nach_oben-24292f?style=for-the-badge)](#top)

</div>

---

<a id="english"></a>

<a id="hardware-english"></a>

# Hardware

This file is the compact hardware reference for the current prototype. It
describes the physically implemented project path; the vendor baseline and
early breadboard, UART, and flex-sensor stages are not active wiring.

The arm is a functional development prototype, but it is not a
safety-certified product. Before any physical movement, the restrictions in
[`SAFETY.md`](../SAFETY.md#english) also apply.

<a id="en-active-components"></a>

## Active components

- Adeept 5-DOF robotic arm with an ATmega328P-based driver board
  (`arduino:avr:uno`)
- five active AD002 servos; the original shoulder servo was replaced with the
  former spare servo, and no additional spare servo remains
- three ESP32-S3-WROOM-1-N16R8 boards: controller, receiver, and a separate
  debug bridge; a fourth board is reserved as a spare
- board revision v1.0 with 16 MB flash, 8 MB octal PSRAM, and an RGB LED on
  `GPIO48`
- three GY-BNO055 modules connected through one PCA9548A multiplexer
- one 10-kOhm potentiometer as the active gripper input
- two Molicel INR-18650-M35A cells in the Adeept battery holder; two additional
  cells as replacements, plus an XTAR VC4SL charger

The custom FQBN for the ESP32 boards is
`esp32:esp32:robotic_arm_s3n16r8`. `GPIO35`, `GPIO36`, and `GPIO37` are occupied
by the octal PSRAM and are not available for project I/O.

<a id="en-controller-sensors"></a>

## Controller and sensors

| Signal | Connection | Counterpart / function |
| --- | --- | --- |
| Gripper ADC | `GPIO1` | potentiometer wiper; the 10-kOhm pull-down remains on the controller |
| I2C SDA | `GPIO8` | PCA9548A SDA |
| I2C SCL | `GPIO9` | PCA9548A SCL |
| IMU status S2 | `GPIO4` | blue LED, upper arm |
| IMU status S1 | `GPIO5` | red LED, forearm |
| IMU status S0 | `GPIO6` | white LED, hand/wrist |
| Communication | `GPIO7` | green LED |
| Fault | `GPIO10` | external FAULT LED |
| Software stop | `GPIO21` | button to GND, internal pull-up |
| RGB | `GPIO48` | onboard fault/stop indicator |

PCA9548A address: `0x70`. All three BNO055 modules use address `0x29` behind
separate multiplexer channels.

| Mux channel | Data index | Body segment |
| --- | --- | --- |
| `0` | `S0` | hand/wrist |
| `1` | `S1` | forearm |
| `2` | `S2` | upper arm |

The potentiometer uses the confirmed two-wire connection: `3.3 V` on one outer
pin and the wiper on `GPIO1`; the existing 10-kOhm pull-down draws the ADC node
to GND. The active calibration anchors are documented in
[`calibration/README.md`](../calibration/README.md).

<a id="en-receiver-arduino-i2c"></a>

## Receiver, Arduino, and I2C

| Signal | ESP32 receiver | Adeept/Arduino board |
| --- | --- | --- |
| SDA | `GPIO13` | `A4`, OLED header pin 3 |
| SCL | `GPIO14` | `A5`, OLED header pin 4 |
| Ground | `GND` | `GND`, common reference |

- ESP32: I2C master at `100 kHz`
- Arduino: I2C slave at `0x42`
- transport: fixed 11-byte Frame V1
- the OLED must remain disconnected while this link is used because it occupies
  the same bus connector
- receiver status: `GPIO4` for I2C, `GPIO5` for ESP-NOW/LINK, and `GPIO48` for RGB

The active definitions are in
[`i2c_frame.h`](../firmware/esp32_receiver/i2c_frame.h) and
[`arduino_arm.ino`](../firmware/arduino_arm/arduino_arm.ino).

<a id="en-i2c-levels"></a>

### I2C levels: verify before operation

The ESP32-S3 operates at `3.3 V`, while the ATmega328P board operates at `5 V`.
I2C uses open-drain signaling, so the actual pull-ups determine the HIGH level.
A working bench connection does not prove that the voltage at the ESP32 is
within its permitted range. The earlier assumption that the internal ATmega
pull-ups are harmless without level adaptation is not a reliable approval.

Before reconnecting the interface to an ESP32:

1. identify every pull-up on the driver board, OLED header, and additional modules;
2. measure idle SDA and SCL against GND;
3. ensure that both lines are pulled up to `3.3 V` on the ESP32 side and never
   exceed the datasheet range there;
4. if the wiring uses `5 V` or is unknown, install a bidirectional level shifter
   suitable for Standard-mode I2C, or verifiably move the pull-ups to `3.3 V`;
5. only then connect the ESP32, Arduino, and common ground.

Applying `5 V` directly to an ESP32-S3 GPIO is not approved. Until the pull-up
voltage and idle levels are documented, this electrical interface remains an
open safety item.

<a id="en-servo-connections-limits"></a>

## Servo connections and active limits

The physical assignment differs from the Adeept stock mapping. The active
firmware maps the byte range `0..255` to the measured minimum and maximum values
for each joint.

| Joint | PWM pin | Stock potentiometer | Min | Max | active start/fallback value |
| --- | ---: | ---: | ---: | ---: | ---: |
| Base | `D9` | `A0` | 12 degrees | 139 degrees | 75 degrees |
| Shoulder | `D11` | `A6` | 35 degrees | 142 degrees | 88 degrees |
| Elbow | `D3` | `A3` | 80 degrees | 175 degrees | 127 degrees |
| Wrist | `D5` | `A2` | 5 degrees | 177 degrees | 91 degrees |
| Gripper | `D6` | `A1` | 32 degrees | 126 degrees | 79 degrees |

The values `75/88/127/91/79` are the integer midpoints of the active limits.
`90 degrees` was a historical mounting and calibration state, but it is not the
current start, timeout, or software-stop pose. The limits were determined
empirically on the prototype; they are not a normative safety verification.

<a id="en-debug-bridge"></a>

## Debug bridge

The bridge is a separate observation node and is not part of the motion path.

| Signal | Pin |
| --- | --- |
| WiFi status | `GPIO4` |
| ESP-NOW status | `GPIO5` |
| MQTT status | `GPIO7` |
| RGB | `GPIO48` |

Its failure must not change the
`Controller -> Receiver -> Arduino -> Servos` path.

<a id="en-power-supply"></a>

## Power supply

- Multi-servo motion is not approved through the USB upload power path.
- The confirmed prototype path uses two qualified 18650 cells in the Adeept
  battery holder; USB is used for uploads and diagnostics.
- Servo load power and sensitive logic power should be planned separately, but
  their signals require a deliberately established common ground reference.
- Separate supplies must not be connected in parallel unintentionally through
  USB, headers, or a breadboard.
- Current draw, fuse protection, conductor sizing, connectors, and strain relief
  still require verification before continuous operation.

<a id="en-primary-sources"></a>

## Primary sources

- [Active controller firmware](../firmware/esp32_controller/esp32_controller.ino)
- [Active receiver firmware](../firmware/esp32_receiver/esp32_receiver.ino)
- [Active Arduino firmware](../firmware/arduino_arm/arduino_arm.ino)
- [ESP32-S3 Series Datasheet](https://documentation.espressif.com/esp32_s3_datasheet_en.pdf)
  for supply and GPIO voltage limits
- [Arduino UNO Rev3](https://store.arduino.cc/products/arduino-uno-rev3) for the
  ATmega328P and its `5 V` operating voltage
- [Microchip TWI example](https://onlinedocs.microchip.com/oxy/GUID-317042D4-BCCE-4065-BB05-AC4312DBC2C4-en-US-2/GUID-DEDA6A06-3138-45C3-BB95-74E4CF6BC406.html)
  for the open-drain bus and required pull-ups

<div align="center">

[![Back to top](https://img.shields.io/badge/⬆_Back_to_top-24292f?style=for-the-badge)](#top)

</div>
