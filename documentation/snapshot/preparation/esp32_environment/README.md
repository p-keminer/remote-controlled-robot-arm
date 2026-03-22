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
| Onboard RGB-LED | `GPIO48` (v1.0-spezifisch, nicht als Hauptanzeigepfad eingeplant) |
| Flash | 16 MB |
| PSRAM | 8 MB Octal (OPI) |
| GPIO35, GPIO36, GPIO37 | intern belegt durch Octal-PSRAM — nicht frei nutzbar |
| Verfuegbare Boards | 4 (2 aktiv als Sender/Receiver, 2 Reserve) |

## Toolchain-Rolle in diesem Projekt

Das Projekt nutzt **Arduino IDE als Hauptumgebung**.
PlatformIO bleibt als dokumentierter Fallback-, Build- und Gegencheckpfad erhalten.

- Hauptumgebung: Arduino IDE auf Windows
- Fallback/Gegencheck: VS Code + PlatformIO Extension
- WSL2: zusaetzlich fuer Dokumentations-Skripte wie `update_docs.sh` und optionale PlatformIO-Laeufe
- Framework-Basis: Arduino-ESP32-Core

Warum Arduino IDE als Hauptumgebung:

- das ist der reale Arbeitsweg fuer Uploads und schnelle Hardwaretests
- Board Manager, Bibliotheken und serielle Gegenpruefungen sind direkt verfuegbar
- die `.ino`-Struktur passt zum tatsaechlichen Projektalltag

Warum PlatformIO trotzdem dokumentiert bleibt:

- `platformio.ini` haelt Board-, Flash- und PSRAM-Konfiguration nachvollziehbar zusammen
- Build- und Konfigurationsfragen lassen sich damit sauber gegenpruefen
- der Pfad bleibt nuetzlich fuer spaetere Validierung oder Notfall-Fallback

## ESP32-Grundlagen fuer dieses Projekt

### Zusaetzlicher PlatformIO-Referenzpfad

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

## Bestaetigte Toolchain- und Stack-Basis (Stand 2026-03-22)

| Ebene | Stand | Status | Hinweis |
| --- | --- | --- | --- |
| Arduino IDE | 3.3.7 | bestaetigt | Hauptumgebung fuer den Projektalltag |
| Arduino-ESP32 Core | 3.3.7 | bestaetigt | dokumentierter Framework-Stand |
| `boards.local.txt` fuer N16R8 | vorhanden | bestaetigt | Flash/PSRAM-Parameter nachgezogen |
| PlatformIO Extension | v3.3.4 | bestaetigt | lokaler Fallback- und Gegencheckpfad vorhanden |
| PlatformIO Platform | `espressif32` v6.13.0 | bestaetigt | fuer N16R8-Bench zusaetzlich nutzbar |
| optionale Komponente `espressif/esp-now` | nicht verwendet | nicht relevant | Advisory-Lage nur bei expliziter Nutzung zusaetzlich pruefen |

Die committed Zielbasis fuer Bench und spaeteren Realbetrieb wird in diesem Dokument gepflegt.
Lokal geflashte Ist-Versionen und Freigabenotizen gehoeren zusaetzlich nach `../../security/local/stack_versions.local.md` oder in gleichwertige gitignorierte Dateien.

### Erwartete Bibliotheksbasis

| Bibliothek | Zweck | Stand |
| --- | --- | --- |
| ESP-NOW (im ESP32 Arduino Core) | Funkpfad Sender → Receiver | Hello-World und Unicast-Bench validiert |
| `Adafruit BNO055` | IMU-Auslesung | installiert und mit GY-BNO055-Boards validiert |
| `Adafruit Unified Sensor` | Abhaengigkeit fuer BNO055 | installiert |
| `Wire` (im Arduino Core) | I2C-Basis fuer IMUs und Mux | mit GPIO8/GPIO9, PCA9548A und BNO055 validiert |
| PCA9548A-Ansteuerung | I2C-Mux fuer mehrere IMUs | manuelle Wire-Ansteuerung validiert, keine Zusatzbibliothek noetig |

**Hinweis BNO055:** Die vorhandenen Module sind **GY-BNO055-Clones** (nicht Adafruit-Breakout).
Die Adafruit-Bibliothek kann verwendet werden, da der Chip identisch ist (I2C-Adressen 0x28 / 0x29).
Adafruit-spezifische Schaltungshinweise aus der Adafruit-Doku gelten jedoch nur eingeschraenkt — Unterschiede auf der Breakout-Platine moeglich.

## Checkliste erste Entwicklungsbereitschaft

- [x] Arduino IDE 3.3.7 auf Windows installiert und einsatzbereit
- [x] ESP32 Board Manager / Arduino-ESP32-Core 3.3.7 dokumentiert
- [x] PlatformIO Extension in VS Code installiert (v3.3.4, WSL-Extension, 2026-03-22)
- [x] `espressif32` Platform in PlatformIO heruntergeladen (v6.13.0)
- [x] Board `esp32-s3-devkitc-1` in PlatformIO erkannt
- [x] Bringup-Firmware geflasht und verifiziert: Flash 16 MB, PSRAM 8189 KB (2026-03-22)
- [x] Serielle Ausgabe ueber USB CDC lesbar (ARDUINO_USB_CDC_ON_BOOT=1)
- [x] N16R8-Parameter eingefroren: qio_opi + BOARD_HAS_PSRAM (siehe firmware/esp32_controller/platformio.ini)
- [x] USB-Bridging WSL2 via usbipd-win eingerichtet (usbipd-win v5.3.0)
- [x] ESP-NOW Hello-World zwischen zwei Boards uebertragen
- [x] ESP-NOW Unicast mit `ImuPaket v1` bench-validiert
- [x] BNO055-Einzeltest, Mux und Dual-IMU-Pfad bench-validiert
- [x] Flex-Sensor ADC-Pfad auf GPIO1 bench-validiert
- [ ] Zielbasis fuer Realbetrieb gegen bekannte Advisories freigeben

## Abgrenzung zu verwandten Dokumenten

- GPIO-Auswahl und Pinplan: `../../hardware/electronics/PIN_MAPPING_RESEARCH.md`
- Bench-Aufbau und Testreihenfolge: `../bench_setup/README.md`
- Arduino IDE als Hauptumgebung: `../arduino_ide_setup/README.md`
- Firmware-Struktur Controller: `../../firmware/esp32_controller/README.md`
- Firmware-Struktur Receiver: `../../firmware/esp32_receiver/README.md`

## Regeln

- Arduino-IDE-Board- und Bibliotheksstand nachvollziehbar halten
- PlatformIO-Konfiguration (`platformio.ini`) als Fallback- und Gegencheckpfad versionieren
- keine impliziten WLAN-Abhaengigkeiten dokumentieren
- N16R8-spezifische Build-Einstellungen klar von generischen ESP32-S3-Einstellungen trennen
- reale Verifikationsschritte nur dann als erledigt markieren, wenn sie auf echter Hardware durchgefuehrt wurden
- committed Zielbasis und lokal geflashte Ist-Versionen getrennt dokumentieren

## Schnittstellen/Abhaengigkeiten

- bezieht sich auf `../../firmware/esp32_controller/`, `../../firmware/esp32_receiver/`, `../../hardware/electronics/` und `../../security/`

## Recherchequellen

- [ESP32-S3-DevKitC-1 User Guide](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide.html) fuer Boardkontext, DevKit-Rolle und offizielle Hardware-Einordnung.
- [ESP32-S3 Series Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf) fuer S3-/PSRAM-Hintergrund und generelle Plattformgrenzen.
- [Adafruit BNO055 Guide](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-bno055-absolute-orientation-sensor.pdf) fuer Bibliotheks- und Chip-Hintergrund zum BNO055 trotz Clone-Hardware.
