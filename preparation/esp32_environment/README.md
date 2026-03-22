# preparation/esp32_environment

## Zweck

Dieser Ordner beschreibt die vorbereitende Umgebung fuer die beiden ESP32-Komponenten.
Er bildet das Bindeglied zwischen bestaetigten Hardware-Fakten, Toolchain-Entscheidungen und dem Weg zum ersten funktionaehigen Bench-Aufbau.

## Bestaetigter Hardware-Stand

Grundlage fuer alle Angaben in diesem Ordner (Stand 2026-03-22):

| Merkmal | Wert |
| --- | --- |
| Modul | `ESP32-S3-WROOM-1-N16R8` |
| Boardrevision | v1.0 |
| Onboard RGB-LED | `GPIO48` (v1.0-spezifisch, nicht als Primaerpfad eingeplant) |
| Flash | 16 MB |
| PSRAM | 8 MB Octal (OPI) |
| GPIO35, GPIO36, GPIO37 | intern belegt durch Octal-PSRAM — nicht frei nutzbar |
| Verfuegbare Boards | 4 (2 aktiv als Sender/Receiver, 2 Reserve) |

## Primärer Toolchain-Pfad: PlatformIO auf Windows

Das Projekt nutzt **PlatformIO als primaeren Entwicklungsweg**.

- Entwicklungsumgebung: VS Code + PlatformIO Extension (Windows)
- Flashing: direkt ueber Windows via USB-C, kein WSL2-Bridging
- WSL2: nur fuer Dokumentations-Skripte wie `update_docs.sh`
- Framework: Arduino-Kompatibilitaetslayer innerhalb PlatformIO

Warum PlatformIO statt Arduino IDE:

- `platformio.ini` als versionierbare, explizite Konfigurationsdatei
- Bibliotheksabhaengigkeiten klar und reproduzierbar
- Board- und PSRAM-Konfiguration ohne manuelle Boardmanager-Eingriffe
- Arduino IDE bleibt als dokumentierter Fallback-Pfad in `../arduino_ide_setup/`

## ESP32-Grundlagen fuer dieses Projekt

### Benoet igte PlatformIO-Plattform

```ini
[env:esp32-s3-devkitc-1]
platform  = espressif32
board     = esp32-s3-devkitc-1
framework = arduino
```

Wichtig fuer N16R8: Flash-Size und PSRAM-Typ muessen explizit gesetzt werden.
Bestaetigt und eingefroren (2026-03-22):

```ini
board_upload.flash_size         = 16MB
board_build.flash_mode          = qio
board_build.arduino.memory_type = qio_opi
build_flags =
    -DBOARD_HAS_PSRAM
    -DARDUINO_USB_CDC_ON_BOOT=1
```

Ergebnis: Flash 16 MB, PSRAM 8189 KB erkannt, USB CDC Serial aktiv.

### Erwartete Bibliotheksbasis

| Bibliothek | Zweck | Stand |
| --- | --- | --- |
| ESP-NOW (im ESP32 Arduino Core) | Funkpfad Sender → Receiver | noch nicht getestet |
| BNO055-Bibliothek | IMU-Auslesung | noch nicht installiert |
| `Wire` (im Arduino Core) | I2C-Basis fuer IMUs und Mux | noch nicht getestet |
| PCA9548A-Ansteuerung | I2C-Mux fuer mehrere IMUs | Bibliothek oder manuelle Ansteuerung noch offen |

**Hinweis BNO055:** Die vorhandenen Module sind **GY-BNO055-Clones** (nicht Adafruit-Breakout).
Die Adafruit-Bibliothek kann verwendet werden, da der Chip identisch ist (I2C-Adressen 0x28 / 0x29).
Adafruit-spezifische Schaltungshinweise aus der Adafruit-Doku gelten jedoch nur eingeschraenkt — Unterschiede auf der Breakout-Platine moeglich.

## Checkliste erste Entwicklungsbereitschaft

- [x] VS Code auf Windows installiert und aktuell
- [x] PlatformIO Extension in VS Code installiert (v3.3.4, WSL-Extension, 2026-03-22)
- [x] `espressif32` Platform in PlatformIO heruntergeladen (v6.13.0)
- [x] Board `esp32-s3-devkitc-1` in PlatformIO erkannt
- [x] Bringup-Firmware geflasht und verifiziert: Flash 16 MB, PSRAM 8189 KB (2026-03-22)
- [x] Serielle Ausgabe ueber USB CDC lesbar (ARDUINO_USB_CDC_ON_BOOT=1)
- [x] N16R8-Parameter eingefroren: qio_opi + BOARD_HAS_PSRAM (siehe firmware/esp32_controller/platformio.ini)
- [x] USB-Bridging WSL2 via usbipd-win eingerichtet (usbipd-win v5.3.0)
- [ ] ESP-NOW Hello-World zwischen zwei Boards uebertragen

## Abgrenzung zu verwandten Dokumenten

- GPIO-Auswahl und Pinplan: `../../hardware/electronics/PIN_MAPPING_RESEARCH.md`
- Bench-Aufbau und Testreihenfolge: `../bench_setup/README.md`
- Arduino IDE als Fallback: `../arduino_ide_setup/README.md`
- Firmware-Struktur Controller: `../../firmware/esp32_controller/README.md`
- Firmware-Struktur Receiver: `../../firmware/esp32_receiver/README.md`

## Regeln

- PlatformIO-Konfiguration (`platformio.ini`) versionieren
- keine impliziten WLAN-Abhaengigkeiten dokumentieren
- N16R8-spezifische Build-Einstellungen klar von generischen ESP32-S3-Einstellungen trennen
- reale Verifikationsschritte nur dann als erledigt markieren, wenn sie auf echter Hardware durchgefuehrt wurden

## Schnittstellen/Abhaengigkeiten

- bezieht sich auf `../../firmware/esp32_controller/`, `../../firmware/esp32_receiver/`, `../../hardware/electronics/` und `../../security/`
