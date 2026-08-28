<a id="top"></a>

<div align="center">

[![Deutsch](https://img.shields.io/badge/🇩🇪_Deutsch-24292f?style=for-the-badge)](#deutsch)
[![English](https://img.shields.io/badge/🇬🇧_English-24292f?style=for-the-badge)](#english)

</div>

---

<a id="deutsch"></a>
<a id="safety"></a>

# Safety

Der Roboterarm kann Personen verletzen und Bauteile beschaedigen. Der aktuelle Stand ist ein getesteter `Entwicklungsprototyp`, aber weder sicherheitszertifiziert noch fuer unbeaufsichtigten, industriellen, medizinischen oder anderweitig sicherheitskritischen Betrieb freigegeben.

Diese Datei beschreibt das tatsaechliche Verhalten der aktiven Firmware. Eine Quellcodefreigabe ist keine Betriebsfreigabe.

<a id="de-kein-echter-not-aus"></a>

## Wichtigste Grenze: kein echter Not-Aus

Der Taster an `GPIO21` ist nur ein **Software-Stop-Toggle**. Er ist kein `fail-safe` Not-Aus und keine Leistungsabschaltung:

- der Controller startet `notaus_aktiv` bei jedem Boot explizit mit `false`;
- ein offener oder nicht angeschlossener Eingang liegt durch `INPUT_PULLUP` auf HIGH und startet damit **freigegeben**, nicht gestoppt;
- auch ein beim Boot gedrueckt gehaltener Taster verriegelt den Stop nicht verlaesslich, weil erst eine spaetere fallende Flanke den Zustand toggelt;
- der Zustand wird per ESP-NOW als Flag uebertragen und kann durch einen zweiten Tastendruck wieder aufgehoben werden;
- es gibt keine redundante Leitung, keine zwangsgefuehrten Kontakte, keine Rueckfuehrung und keinen Totmannpfad;
- Servos bleiben elektrisch versorgt und attached; es wird weder Leistung noch Drehmoment abgeschaltet.

Ein realer Betrieb braucht deshalb einen unabhaengigen, gut erreichbaren und verriegelnden Hardware-Leistungstrenner fuer den Servostrom. Dieser ist im Repository-Stand nicht vorhanden.

<a id="de-verhalten-je-receiver-modus"></a>

### Verhalten je Receiver-Modus

| Modus | Reaktion auf das Controller-Flag |
| --- | --- |
| Live-I2C | Receiver sendet `FRAME_FLAG_ESTOP`; der Arduino setzt neue Zielwerte auf seine Mittelpositionen und faehrt sie mit Slew-Limit an. Das ist kein sofortiges stromloses Stoppen. |
| committed Default `TEST_SWEEP` | Das Flag aendert LEDs/Status, der Sweep-Generator sendet aber weiter gueltige Bewegungsframes. Der GPIO21-Taster stoppt den Test-Sweep **nicht**. |

Vor jedem Einschalten muss deshalb bekannt sein, welcher Receiver-Build wirklich geflasht ist. Der committed Default ist `RECEIVER_MODE_TEST_SWEEP=1` und `RECEIVER_MODE_LIVE_I2C=0`; Live-Betrieb und jede Achse werden nur ueber explizite Compile-Flags freigegeben.

<a id="de-rueckfallpose"></a>

## Start-, Stop- und Rueckfallpose

Die aktive Arduino-Firmware verwendet die ganzzahligen Mittelpunkte der realen Grenzen. Sie verwendet nicht pauschal `90 Grad`.

| Gelenk | empirische Grenzen | Start / Timeout / Software-Stop |
| --- | --- | ---: |
| Basis | 12 bis 139 Grad | 75 Grad |
| Schulter | 35 bis 142 Grad | 88 Grad |
| Ellbogen | 80 bis 175 Grad | 127 Grad |
| Handgelenk | 5 bis 177 Grad | 91 Grad |
| Greifer | 32 bis 126 Grad | 79 Grad |

Diese Pose ist eine softwareseitige Mittelstellung, nicht nachgewiesen kollisionsfrei und nicht automatisch ergonomisch oder mechanisch lastfrei. Beim Attach kann die Arduino-Servo-Library kurz ihren Defaultpuls fuer `90 Grad` ausgeben, bevor der Sketch den jeweiligen Startwert schreibt. Servoenergie darf daher nicht in einer unkontrollierten oder kollisionsnahen Aufstellung zugeschaltet werden.

<a id="de-zeit-und-rampen"></a>

## Zeit- und Rampenverhalten

| Ebene | aktuelles Verhalten |
| --- | --- |
| Receiver Live-I2C | Paket aelter als `250 ms` fuehrt zu `FRAME_FLAG_NEUTRAL`. |
| Receiver Linkanzeige | Nach `2000 ms` ohne ESP-NOW-Paket wird ein Linkfehler angezeigt; dies ist allein keine Leistungsabschaltung. |
| Arduino | Nach `1000 ms` ohne gueltiges I2C-Frame werden die Mittelpositionen angefordert. |
| Arduino-Rampe | maximal `3 Grad` pro `20 ms`, rechnerisch bis etwa `150 Grad/s` je Achse. |
| Receiver-Rampe | pro 20-ms-Frame maximal 2 Byte fuer Basis/Schulter, 8 fuer Ellbogen/Handgelenk und 12 fuer Greifer. |

`ESTOP` und `NEUTRAL` springen nicht unmittelbar auf die Rueckfallpose; auch sie laufen durch die Arduino-Rampe. Fehlerhafte Magics, Versionen oder XOR-Werte werden verworfen. Die als `crc8` bezeichnete Pruefung ist technisch nur eine XOR-Pruefsumme und weder eine Safety-Garantie noch eine Authentisierung.

<a id="de-mechanische-grenzen"></a>

## Mechanische und regelungstechnische Grenzen

- Die Gelenkgrenzen sind empirische Prototypwerte, keine zertifizierten Sicherheitsgrenzen.
- Die Servos haben kein Positions-, Kraft- oder Stromfeedback. Die Software kennt weder reale Istposition noch Blockade oder externe Last.
- Der Realarm-Pfad nutzt Eulerwerte und pragmatische relative Abbildungen, keine vollstaendige Kinematik.
- Es gibt keinen nachgewiesenen Kollisionsschutz; der Dashboard-Solver ist kein Teil des Steuerpfads und ersetzt keinen realen Schutz.
- Den Arm niemals bei eingeschalteter Servoenergie von Hand erzwingen.
- Montage, Kabel, Perfboard und Wearable sind nicht auf Schweiss, Zug, Sturz, Dauerlast oder Alltagstauglichkeit freigegeben.

Die aktiven Grenzen und das Realarm-Mapping stehen in [`calibration/README.md`](calibration/README.md). Die reale Verdrahtung steht in [`docs/HARDWARE.md`](docs/HARDWARE.md#deutsch).

<a id="de-i2c-risiko"></a>

## Elektrisches I2C-Risiko

Der Receiver-ESP32 arbeitet mit `3.3 V`, das ATmega328P-Board mit `5 V`. SDA und SCL sind Open-Drain-Leitungen; vorhandene Pull-ups bestimmen den HIGH-Pegel. Ein Pull-up auf `5 V` kann den ESP32-S3 ausserhalb seines zulaessigen GPIO-Bereichs betreiben und beschaedigen.

Vor realer Bewegung muessen Pull-up-Ziel, Leerlaufpegel und gemeinsame Masse gemessen und dokumentiert werden. Die Leitungen muessen auf der ESP32-Seite auf `3.3 V` gezogen werden; bei `5 V` oder unbekannter Boardbeschaltung ist ein geeigneter bidirektionaler I2C-Levelshifter Pflicht. Bis dieser Nachweis vorliegt, ist die Verbindung `GPIO13/GPIO14 -> A4/A5` nicht elektrisch freigegeben. Details und Primaerquellen stehen in [`docs/HARDWARE.md`](docs/HARDWARE.md#de-i2c-pegel).

<a id="de-versorgung"></a>

## Versorgung

- USB ist Upload-/Diagnosepfad und keine freigegebene Quelle fuer fuenf gleichzeitig aktive Servos.
- Servopfad und Logikpfade muessen definiert versorgt werden; gemeinsame Masse nur bewusst herstellen.
- Keine Versorgungen unbeabsichtigt ueber USB, Header oder Breadboard parallelschalten.
- Nur qualifizierte, unbeschaedigte 18650-Zellen paarweise mit gleichem Ladezustand verwenden und ausserhalb des Geraets in einem geeigneten Ladegeraet laden.
- Stromaufnahme, Sicherung, Kabelquerschnitt und Stecker sind fuer Dauerbetrieb noch nicht nachgewiesen.

<a id="de-mindestablauf"></a>

## Mindestablauf vor Bewegung

1. Arm standsicher befestigen; Arbeitsraum, Quetschstellen und Anschlaege frei halten. Keine Person im Bewegungsraum.
2. Unabhaengigen Servostrom-Trenner erreichbar halten; Software-Stop nicht als einzige Schutzmassnahme verwenden.
3. I2C-Pegel nach [`docs/HARDWARE.md`](docs/HARDWARE.md#deutsch) verifizieren.
4. Geflashten Receiver-Modus und alle Achsenfreigaben ueber serielle Ausgabe pruefen. Test-Sweep nur in dafuer vorbereiteter, freier Aufstellung starten.
5. Zuerst ohne Servolast bzw. mit getrenntem Servostrom Kommunikation, Paketfrische und erwartete Zielwerte pruefen.
6. Beim ersten Zuschalten der Servoenergie mit kleinen, einzeln freigegebenen Bewegungen beginnen; jederzeit Hardware-Leistung trennen koennen.
7. Nach Reset, Reflash, Kabelwechsel, Kalibrierung oder mechanischem Umbau die Freigabe von vorn durchlaufen.

<a id="de-offene-freigabepunkte"></a>

## Offene Freigabepunkte

- fail-safe Hardware-Not-Aus bzw. Servostrom-Trennung
- verifizierter 3.3-V-I2C-Pegel oder Levelshifter
- formale Tests fuer Boot, Stop, Timeout, Paketverlust und Wiederanlauf
- Last-, Blockier-, Dauerlauf- und Missbrauchstests
- dokumentierter Strompfad mit Absicherung
- Kollisions- und Quetschstellenbewertung
- Security-Haertung des Funkpfads; XOR ist kein Manipulationsschutz

Solange diese Punkte offen sind, bleibt der Arm ein beaufsichtigter Entwicklungsdemonstrator.

<a id="de-implementierungsquellen"></a>

## Implementierungsquellen

- [Controller: GPIO21 und Startzustand](firmware/esp32_controller/esp32_controller.ino)
- [Receiver: Modi, Stale- und ESTOP-Abbildung](firmware/esp32_receiver/esp32_receiver.ino)
- [Arduino: Limits, Timeout und Slew](firmware/arduino_arm/arduino_arm.ino)
- [ESP32-S3 Series Datasheet](https://documentation.espressif.com/esp32_s3_datasheet_en.pdf)
- [Arduino UNO Rev3](https://store.arduino.cc/products/arduino-uno-rev3)

<div align="center">

[![Nach oben](https://img.shields.io/badge/⬆_Nach_oben-24292f?style=for-the-badge)](#top)

</div>

---

<a id="english"></a>
<a id="safety-english"></a>

# Safety

The robot arm can injure people and damage components. The current state is a tested `development prototype`, but it is neither safety-certified nor approved for unattended, industrial, medical, or any other safety-critical operation.

This file describes the actual behavior of the active firmware. Source-code approval is not operational approval.

<a id="en-no-real-emergency-stop"></a>

## Most important limitation: no real emergency stop

The button on `GPIO21` is only a **software stop toggle**. It is not a `fail-safe` emergency stop and does not disconnect power:

- the controller explicitly initializes `notaus_aktiv` to `false` on every boot;
- because of `INPUT_PULLUP`, an open or disconnected input is HIGH and therefore starts **enabled**, not stopped;
- even a button held during boot does not reliably latch the stop, because only a later falling edge toggles the state;
- the state is transmitted as a flag over ESP-NOW and can be cleared again by a second button press;
- there is no redundant circuit, no forcibly guided contacts, no feedback circuit, and no dead-man path;
- the servos remain electrically powered and attached; neither power nor torque is removed.

Real operation therefore requires an independent, easily reachable, and latching hardware power disconnect for the servo supply. It is not present in the current repository state.

<a id="en-behavior-by-receiver-mode"></a>

### Behavior by receiver mode

| Mode | Reaction to the controller flag |
| --- | --- |
| Live-I2C | The receiver sends `FRAME_FLAG_ESTOP`; the Arduino sets new targets to its midpoint positions and approaches them through the slew limit. This is not an immediate unpowered stop. |
| committed default `TEST_SWEEP` | The flag changes LEDs/status, but the sweep generator continues to send valid motion frames. The GPIO21 button does **not** stop the test sweep. |

Before every power-on, the receiver build that is actually flashed must therefore be known. The committed default is `RECEIVER_MODE_TEST_SWEEP=1` and `RECEIVER_MODE_LIVE_I2C=0`; live operation and every axis are enabled only through explicit compile flags.

<a id="en-fallback-pose"></a>

## Start, stop, and fallback pose

The active Arduino firmware uses the integer midpoints of the real limits. It does not use a blanket `90 degrees` value.

| Joint | Empirical limits | Start / timeout / software stop |
| --- | --- | ---: |
| Base | 12 to 139 degrees | 75 degrees |
| Shoulder | 35 to 142 degrees | 88 degrees |
| Elbow | 80 to 175 degrees | 127 degrees |
| Wrist | 5 to 177 degrees | 91 degrees |
| Gripper | 32 to 126 degrees | 79 degrees |

This pose is a software midpoint position; it has not been proven collision-free and is not automatically ergonomic or mechanically unloaded. On attach, the Arduino Servo library may briefly output its default pulse for `90 degrees` before the sketch writes the respective start value. Servo power must therefore not be switched on while the arm is in an uncontrolled or near-collision setup.

<a id="en-timing-and-ramps"></a>

## Timing and ramp behavior

| Layer | Current behavior |
| --- | --- |
| Receiver Live-I2C | A packet older than `250 ms` leads to `FRAME_FLAG_NEUTRAL`. |
| Receiver link indication | After `2000 ms` without an ESP-NOW packet, a link error is indicated; this alone does not disconnect power. |
| Arduino | After `1000 ms` without a valid I2C frame, the midpoint positions are requested. |
| Arduino ramp | maximum `3 degrees` per `20 ms`, mathematically up to about `150 degrees/s` per axis. |
| Receiver ramp | per 20 ms frame, at most 2 bytes for base/shoulder, 8 for elbow/wrist, and 12 for gripper. |

`ESTOP` and `NEUTRAL` do not jump immediately to the fallback pose; they also pass through the Arduino ramp. Frames with invalid magic values, versions, or XOR values are discarded. The check named `crc8` is technically only an XOR checksum and provides neither a safety guarantee nor authentication.

<a id="en-mechanical-limits"></a>

## Mechanical and control-system limitations

- The joint limits are empirical prototype values, not certified safety limits.
- The servos have no position, force, or current feedback. The software knows neither the actual position nor a stall or external load.
- The real-arm path uses Euler values and pragmatic relative mappings, not complete kinematics.
- There is no proven collision protection; the dashboard solver is not part of the control path and does not replace real protection.
- Never force the arm by hand while servo power is enabled.
- The mounting, cables, perfboard, and wearable are not approved for sweat, pulling, falls, continuous load, or everyday use.

The active limits and real-arm mapping are documented in [`calibration/README.md`](calibration/README.md). The actual wiring is documented in [`docs/HARDWARE.md`](docs/HARDWARE.md#english).

<a id="en-i2c-risk"></a>

## Electrical I2C risk

The receiver ESP32 operates at `3.3 V`, while the ATmega328P board operates at `5 V`. SDA and SCL are open-drain lines; the installed pull-ups determine the HIGH level. A pull-up to `5 V` can operate the ESP32-S3 outside its permitted GPIO range and damage it.

Before real motion, the pull-up target, idle level, and common ground must be measured and documented. The lines must be pulled to `3.3 V` on the ESP32 side; with `5 V` or unknown board wiring, a suitable bidirectional I2C level shifter is mandatory. Until this evidence exists, the `GPIO13/GPIO14 -> A4/A5` connection is not electrically approved. Details and primary sources are provided in [`docs/HARDWARE.md`](docs/HARDWARE.md#en-i2c-levels).

<a id="en-power-supply"></a>

## Power supply

- USB is an upload/diagnostic path and is not an approved source for five simultaneously active servos.
- The servo path and logic paths must have defined supplies; establish a common ground only deliberately.
- Do not unintentionally parallel power supplies through USB, headers, or a breadboard.
- Use only qualified, undamaged 18650 cells in matched pairs at the same state of charge, and charge them outside the device in a suitable charger.
- Current draw, fuse protection, cable cross-section, and connectors have not yet been demonstrated for continuous operation.

<a id="en-minimum-procedure"></a>

## Minimum procedure before motion

1. Secure the arm so it is stable; keep the workspace, pinch points, and stops clear. No person may be inside the motion area.
2. Keep an independent servo-power disconnect within reach; do not use the software stop as the only protective measure.
3. Verify the I2C levels according to [`docs/HARDWARE.md`](docs/HARDWARE.md#english).
4. Check the flashed receiver mode and every axis enable through serial output. Start the test sweep only in a prepared, unobstructed setup.
5. First verify communication, packet freshness, and expected targets without servo load or with servo power disconnected.
6. When enabling servo power for the first time, begin with small movements and individually enabled axes; hardware power must remain disconnectable at all times.
7. After a reset, reflash, cable change, calibration, or mechanical modification, repeat the approval procedure from the beginning.

<a id="en-open-approval-items"></a>

## Open approval items

- fail-safe hardware emergency stop or servo-power disconnect
- verified 3.3 V I2C level or level shifter
- formal tests for boot, stop, timeout, packet loss, and restart
- load, stall, endurance, and misuse tests
- documented power path with protection
- collision and pinch-point assessment
- security hardening of the radio path; XOR does not protect against manipulation

As long as these items remain open, the arm remains a supervised development demonstrator.

<a id="en-implementation-sources"></a>

## Implementation sources

- [Controller: GPIO21 and initial state](firmware/esp32_controller/esp32_controller.ino)
- [Receiver: modes, stale-data, and ESTOP mapping](firmware/esp32_receiver/esp32_receiver.ino)
- [Arduino: limits, timeout, and slew](firmware/arduino_arm/arduino_arm.ino)
- [ESP32-S3 Series Datasheet](https://documentation.espressif.com/esp32_s3_datasheet_en.pdf)
- [Arduino UNO Rev3](https://store.arduino.cc/products/arduino-uno-rev3)

<div align="center">

[![Back to top](https://img.shields.io/badge/⬆_Back_to_top-24292f?style=for-the-badge)](#top)

</div>
