# preparation/arduino_ide_setup

## Zweck

Dieser Ordner dokumentiert die Arduino IDE als Hauptumgebung fuer Upload, lokale Entwicklung und schnelle Hardwaretests.

PlatformIO bleibt als Fallback- und Gegencheckpfad dokumentiert.
Arduino IDE ist der bevorzugte Arbeitsweg fuer folgende Szenarien:

- regulaere Sketch-Arbeit und Uploads
- schnelle Einzeltests direkt am Board
- Bibliotheks- und Boardmanager-Workflow fuer den Projektalltag

## Aktueller Stand

- Arduino IDE 3.3.7 ist auf Windows installiert
- ESP32 Board Manager / Arduino-ESP32-Core 3.3.7 ist als Hauptumgebungsstand dokumentiert
- Boardeinstellungen fuer `ESP32S3 Dev Module` und N16R8 sind festgehalten
- Relevante Bench-Ergebnisse koennen ueber Arduino IDE und alternativ ueber PlatformIO gegengeprueft werden
- der offizielle Adeept-Stock-Pfad des Arm-Boards bleibt ueber `Arduino Uno` und CH341-Treiber dokumentiert, auch wenn das Projekt alltagsseitig mit aktueller Arduino IDE arbeitet

## Board Manager URL fuer Espressif ESP32

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Nach dem Eintragen im Board Manager: Espressif ESP32-Core installieren.
Fuer den `ESP32-S3-WROOM-1-N16R8` das Board `ESP32S3 Dev Module` waehlen.

## Inhalt

- Board Manager URL und Grundkonfiguration fuer ESP32-S3
- Bibliotheksreferenz fuer den Alltagsworkflow
- Einfache Pruefpunkte fuer Einsatzbereitschaft
- Herstellerkontext fuer das originale Adeept-Arm-Board

## Regeln

- keine Versionsannahmen ohne Nachweis festschreiben
- reale Installationsstaende nur dokumentieren wenn durchgefuehrt
- Board- und Bibliothekseinstellungen muessen mit `../esp32_environment/README.md` konsistent bleiben
- PlatformIO nur als zusaetzlichen Fallback- und Gegencheckpfad behandeln
- offizieller Herstellerkontext darf dokumentiert werden, ohne die Projekt-Hauptumgebung umzudefinieren

## Schnittstellen/Abhaengigkeiten

- bezieht sich auf `../../firmware/` und `../../docs/templates/`
- ergaenzt `../esp32_environment/README.md`

## Recherchequellen

- [Adeept V4 Package Review](../../official_downloads/ADEEPT_V4_PACKAGE_REVIEW.md) fuer den offiziellen Herstellerpfad `Arduino Uno` und CH341-Treiber beim Stock-Board.
