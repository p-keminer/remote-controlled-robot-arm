<a id="top"></a>

<div align="center">

[![Deutsch](https://img.shields.io/badge/🇩🇪_Deutsch-24292f?style=for-the-badge)](#deutsch)
[![English](https://img.shields.io/badge/🇬🇧_English-24292f?style=for-the-badge)](#english)

</div>

---

<a id="deutsch"></a>

<a id="build-und-flashen"></a>

# Build und Flashen

Diese Anleitung beschreibt die aktiven Firmwareziele. Ein erfolgreicher
`Compile-Check` beweist nur, dass der Quellstand mit der genannten Toolchain
uebersetzt werden kann; er ist keine neue Hardware-, Safety- oder Flash-Freigabe.

<a id="de-ziele"></a>

## Ziele

| Komponente | Aktiver Sketch | Ziel |
| --- | --- | --- |
| Controller | [`firmware/esp32_controller/esp32_controller.ino`](../firmware/esp32_controller/esp32_controller.ino) | ESP32-S3-WROOM-1-N16R8 |
| Receiver | [`firmware/esp32_receiver/esp32_receiver.ino`](../firmware/esp32_receiver/esp32_receiver.ino) | ESP32-S3-WROOM-1-N16R8 |
| Debug-Bridge | [`firmware/esp32_bridge/esp32_bridge.ino`](../firmware/esp32_bridge/esp32_bridge.ino) | ESP32-S3-WROOM-1-N16R8 |
| Servoebene | [`firmware/arduino_arm/arduino_arm.ino`](../firmware/arduino_arm/arduino_arm.ino) | Arduino UNO / ATmega328P |

Dateien unter [`firmware/archive/`](../firmware/archive/) sind historische Referenzen und keine aktiven Build- oder Flashziele.

Der primaere Projektweg bleibt ein Arduino-Sketch ueber `Arduino IDE` oder
`Arduino CLI`. Die vorhandenen `PlatformIO`-Dateien dienen als Fallback und
Build-Gegencheck; Upload-Anleitungen muessen klar nennen, welcher Weg verwendet
wurde.

<a id="de-boarddefinition"></a>

## ESP32-Boarddefinition: bekannte Reproduzierbarkeitsluecke

Der reale Uploadpfad verwendet das lokale Custom-FQBN:

```text
esp32:esp32:robotic_arm_s3n16r8
```

Die zugehoerige `boards.local.txt` liegt derzeit nur in der lokalen Arduino-Installation und ist nicht Bestandteil des Repositories. Ein frischer Clone kann dieses FQBN daher nicht allein aus dem Repo rekonstruieren. Vor einem Upload muss

```powershell
arduino-cli board details -b "esp32:esp32:robotic_arm_s3n16r8"
```

die lokale Definition erfolgreich finden. Fehlt sie, ist der Hardware-Upload blockiert, bis die bestaetigte Boarddefinition separat bereitgestellt wurde.

Fuer den reinen Compile-Check vom 2026-08-28 wurde ersatzweise dieses explizit konfigurierte generische FQBN verwendet:

```text
esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi,CDCOnBoot=cdc
```

Das ist ein Syntax-/Buildnachweis und keine Freigabe, das generische Board auf die reale Hardware zu flashen. Die vorhandenen `platformio.ini`-Dateien fuer [Controller](../firmware/esp32_controller/platformio.ini) und [Receiver](../firmware/esp32_receiver/platformio.ini) bleiben ebenfalls nur Fallback und Gegencheck.

<a id="de-lokale-konfiguration"></a>

## Lokale Konfiguration

Echte MAC-Adressen, Netzwerkdaten und Passwoerter duerfen nicht committed werden. Vor einem lokalen Build die benoetigten Vorlagen kopieren und nur die gitignorierte Kopie bearbeiten:

| Komponente | Vorlage | Lokale Datei |
| --- | --- | --- |
| Controller | [`peer_config.template.h`](../firmware/esp32_controller/peer_config.template.h) | `peer_config.local.h` |
| Bridge | [`peer_config.template.h`](../firmware/esp32_bridge/peer_config.template.h) | `peer_config.local.h` |
| Bridge | [`wifi_config.template.h`](../firmware/esp32_bridge/wifi_config.template.h) | `wifi_config.local.h` |

Im Controller-Template ist `BRIDGE_AKTIV=0`. Fuer den aktiven Debug-/Twin-Pfad muss diese Option ausschliesslich in der lokalen Kopie bewusst auf `1` gesetzt und die Bridge als zweiter Peer eingetragen werden. Platzhalterdateien reichen fuer einen Compile-Check, duerfen aber nicht als betriebsfertige Peer-Konfiguration geflasht werden.

<a id="de-buildstand"></a>

## Nachgewiesener Buildstand vom 2026-08-28

Der Check lief mit `arduino-cli 1.5.1`, ESP32-Core `3.3.5` und AVR-Core `1.8.8`. `arduino-cli 1.5.1` und AVR-Core `1.8.8` waren am Auditdatum die aktuellen stabilen Releases; `arduino-cli 1.5.2-rc.1` war nur ein Pre-Release. Die Libraries wurden aus dem frisch aktualisierten Arduino-Index in einem isolierten temporaeren Verzeichnis bereitgestellt:

| Library | Version | Verwendet von |
| --- | --- | --- |
| Adafruit BNO055 | 1.6.4 | Controller |
| Adafruit Unified Sensor | 1.1.15 | Controller |
| Adafruit BusIO | 1.17.4 | transitive Adafruit-Abhaengigkeit |
| Adafruit NeoPixel | 1.15.5 | Controller und Receiver |
| Servo | 1.3.0 | Arduino-Servoebene |

Der Arduino-Index und das offizielle Espressif-Release meldeten fuer den
ESP32-Core bereits `3.3.11` als aktuelle stabile Version. Sie wurde nur
auditiert, nicht fuer diese Hardware gebaut oder freigegeben; ein Update ist
deshalb keine automatische Upgrade-Empfehlung.

Ergebnis des frischen Compile-Checks:

| Ziel | Flash | RAM | Ergebnis |
| --- | ---: | ---: | --- |
| Controller | 930311 B | 45544 B | erfolgreich |
| Receiver, Repo-Default | 902395 B | 45104 B | erfolgreich |
| Receiver, Live-I2C alle Achsen | 904751 B | 45136 B | erfolgreich |
| Arduino UNO | 4280 B | 282 B | erfolgreich |

Die aktive Debug-Bridge wurde in diesem Audit nicht frisch kompiliert. Aus dem Quellcode folgen zusaetzlich `PubSubClient`, Adafruit NeoPixel sowie die mit dem ESP32-Core gelieferten WiFi-, ESP-NOW- und ArduinoOTA-Komponenten; daraus wird hier kein aktueller Bridge-Builderfolg abgeleitet.

<a id="de-compile-checks"></a>

## Reproduzierbare Compile-Checks

Die folgenden Befehle setzen voraus, dass die oben genannten Libraries installiert oder ueber `--libraries` eingebunden und die erforderlichen lokalen Header vorhanden sind.

```powershell
arduino-cli compile --fqbn "esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi,CDCOnBoot=cdc" firmware/esp32_controller
arduino-cli compile --fqbn "esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi,CDCOnBoot=cdc" firmware/esp32_receiver
arduino-cli compile --fqbn "arduino:avr:uno" firmware/arduino_arm
```

Der committed Receiver-Default ist der Test-Sweep. Der nachgewiesene Live-All-Axes-Build veraendert keine Quelldatei und nutzt exakt diese Build-Flags:

```powershell
arduino-cli compile --fqbn "esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi,CDCOnBoot=cdc" --build-property "compiler.cpp.extra_flags=-DRECEIVER_MODE_TEST_SWEEP=0 -DRECEIVER_MODE_LIVE_I2C=1 -DRECEIVER_LIVE_ENABLE_BASIS=1 -DRECEIVER_LIVE_ENABLE_SHOULDER=1 -DRECEIVER_LIVE_ENABLE_ELBOW=1 -DRECEIVER_LIVE_ENABLE_WRIST=1 -DRECEIVER_LIVE_ENABLE_GRIPPER=1" firmware/esp32_receiver
```

`RECEIVER_I2C_SEND_ENABLED` bleibt dabei auf seinem Quellcode-Default `1`. Fuer einen reinen Mapping-Dry-Run kann es als zusaetzliches Build-Flag auf `0` gesetzt werden; dieser Dry-Run ist nicht Teil des oben protokollierten Checks.

<a id="de-upload"></a>

## Vor einem realen Upload

1. Custom-FQBN und aktuellen Port lokal pruefen.
2. Lokale Peer-/Netzwerkkonfiguration auf das richtige Geraet begrenzen.
3. Receiver-Modus und alle Achsenfreigaben bewusst kontrollieren.
4. Stromversorgung, I2C-Pegel und mechanische Freigabe nach [`HARDWARE.md`](HARDWARE.md#deutsch) und [`SAFETY.md`](../SAFETY.md#deutsch) pruefen.
5. Erst danach mit dem Custom-FQBN flashen und den Testplan aus [`TESTING.md`](TESTING.md#deutsch) abarbeiten.

<div align="center">

[![Nach oben](https://img.shields.io/badge/⬆_Nach_oben-24292f?style=for-the-badge)](#top)

</div>

---

<a id="english"></a>

<a id="build-and-flashing"></a>

# Build and Flashing

This guide describes the active firmware targets. A successful `compile check`
only proves that the source state can be translated with the stated toolchain;
it is not a new hardware, safety, or flashing approval.

<a id="en-targets"></a>

## Targets

| Component | Active sketch | Target |
| --- | --- | --- |
| Controller | [`firmware/esp32_controller/esp32_controller.ino`](../firmware/esp32_controller/esp32_controller.ino) | ESP32-S3-WROOM-1-N16R8 |
| Receiver | [`firmware/esp32_receiver/esp32_receiver.ino`](../firmware/esp32_receiver/esp32_receiver.ino) | ESP32-S3-WROOM-1-N16R8 |
| Debug bridge | [`firmware/esp32_bridge/esp32_bridge.ino`](../firmware/esp32_bridge/esp32_bridge.ino) | ESP32-S3-WROOM-1-N16R8 |
| Servo layer | [`firmware/arduino_arm/arduino_arm.ino`](../firmware/arduino_arm/arduino_arm.ino) | Arduino UNO / ATmega328P |

Files under [`firmware/archive/`](../firmware/archive/) are historical references and are not active build or flash targets.

The primary project workflow remains an Arduino sketch through `Arduino IDE` or
`Arduino CLI`. The existing `PlatformIO` files serve as a fallback and build
cross-check; upload instructions must state clearly which route was used.

<a id="en-board-definition"></a>

## ESP32 Board Definition: Known Reproducibility Gap

The real upload path uses the local custom FQBN:

```text
esp32:esp32:robotic_arm_s3n16r8
```

The corresponding `boards.local.txt` currently exists only in the local Arduino installation and is not part of the repository. A fresh clone therefore cannot reconstruct this FQBN from the repository alone. Before an upload,

```powershell
arduino-cli board details -b "esp32:esp32:robotic_arm_s3n16r8"
```

must find the local definition successfully. If it is missing, hardware upload remains blocked until the confirmed board definition has been supplied separately.

For the compile-only check on 2026-08-28, the following explicitly configured generic FQBN was used instead:

```text
esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi,CDCOnBoot=cdc
```

This is syntax/build evidence and not approval to flash the generic board definition onto the real hardware. The existing `platformio.ini` files for the [controller](../firmware/esp32_controller/platformio.ini) and [receiver](../firmware/esp32_receiver/platformio.ini) likewise remain fallback and cross-check paths only.

<a id="en-local-configuration"></a>

## Local Configuration

Real MAC addresses, network data, and passwords must not be committed. Before a local build, copy the required templates and edit only the gitignored copy:

| Component | Template | Local file |
| --- | --- | --- |
| Controller | [`peer_config.template.h`](../firmware/esp32_controller/peer_config.template.h) | `peer_config.local.h` |
| Bridge | [`peer_config.template.h`](../firmware/esp32_bridge/peer_config.template.h) | `peer_config.local.h` |
| Bridge | [`wifi_config.template.h`](../firmware/esp32_bridge/wifi_config.template.h) | `wifi_config.local.h` |

The controller template sets `BRIDGE_AKTIV=0`. For the active debug/digital-twin path, this option must be deliberately changed to `1` only in the local copy and the bridge must be entered as the second peer. Placeholder files are sufficient for a compile check, but must not be flashed as an operational peer configuration.

<a id="en-build-status"></a>

## Verified Build State on 2026-08-28

The check used `arduino-cli 1.5.1`, ESP32 core `3.3.5`, and AVR core `1.8.8`. `arduino-cli 1.5.1` and AVR core `1.8.8` were the current stable releases on the audit date; `arduino-cli 1.5.2-rc.1` was only a pre-release. The libraries were provided from the freshly updated Arduino index in an isolated temporary directory:

| Library | Version | Used by |
| --- | --- | --- |
| Adafruit BNO055 | 1.6.4 | Controller |
| Adafruit Unified Sensor | 1.1.15 | Controller |
| Adafruit BusIO | 1.17.4 | transitive Adafruit dependency |
| Adafruit NeoPixel | 1.15.5 | Controller and receiver |
| Servo | 1.3.0 | Arduino servo layer |

The Arduino index and the official Espressif release already identified
ESP32 core `3.3.11` as the current stable version. It was audited only, not
built or approved for this hardware; an update is therefore not an automatic
upgrade recommendation.

Result of the fresh compile check:

| Target | Flash | RAM | Result |
| --- | ---: | ---: | --- |
| Controller | 930311 B | 45544 B | successful |
| Receiver, repository default | 902395 B | 45104 B | successful |
| Receiver, live I2C on all axes | 904751 B | 45136 B | successful |
| Arduino UNO | 4280 B | 282 B | successful |

The active debug bridge was not freshly compiled in this audit. Its source additionally requires `PubSubClient`, Adafruit NeoPixel, and the WiFi, ESP-NOW, and ArduinoOTA components supplied with the ESP32 core; no current bridge build success is inferred from that fact.

<a id="en-compile-checks"></a>

## Reproducible Compile Checks

The following commands assume that the libraries listed above are installed or included through `--libraries` and that the required local headers exist.

```powershell
arduino-cli compile --fqbn "esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi,CDCOnBoot=cdc" firmware/esp32_controller
arduino-cli compile --fqbn "esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi,CDCOnBoot=cdc" firmware/esp32_receiver
arduino-cli compile --fqbn "arduino:avr:uno" firmware/arduino_arm
```

The committed receiver default is the test sweep. The verified live-all-axes build does not modify any source file and uses exactly these build flags:

```powershell
arduino-cli compile --fqbn "esp32:esp32:esp32s3:FlashSize=16M,PSRAM=opi,CDCOnBoot=cdc" --build-property "compiler.cpp.extra_flags=-DRECEIVER_MODE_TEST_SWEEP=0 -DRECEIVER_MODE_LIVE_I2C=1 -DRECEIVER_LIVE_ENABLE_BASIS=1 -DRECEIVER_LIVE_ENABLE_SHOULDER=1 -DRECEIVER_LIVE_ENABLE_ELBOW=1 -DRECEIVER_LIVE_ENABLE_WRIST=1 -DRECEIVER_LIVE_ENABLE_GRIPPER=1" firmware/esp32_receiver
```

`RECEIVER_I2C_SEND_ENABLED` remains at its source-code default of `1`. For a mapping-only dry run, it can be set to `0` through an additional build flag; that dry run is not part of the check recorded above.

<a id="en-upload"></a>

## Before a Real Upload

1. Check the custom FQBN and current port locally.
2. Limit the local peer/network configuration to the correct device.
3. Deliberately verify the receiver mode and every axis enable flag.
4. Check the power supply, I2C levels, and mechanical approval against [`HARDWARE.md`](HARDWARE.md#english) and [`SAFETY.md`](../SAFETY.md#english).
5. Only then flash with the custom FQBN and follow the test plan in [`TESTING.md`](TESTING.md#english).

<div align="center">

[![Back to top](https://img.shields.io/badge/⬆_Back_to_top-24292f?style=for-the-badge)](#top)

</div>
