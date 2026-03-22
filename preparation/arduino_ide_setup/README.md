# preparation/arduino_ide_setup

## Zweck

Dieser Ordner dokumentiert die Arduino IDE als **primaeren Flash- und Entwicklungspfad**.

PlatformIO (WSL) bleibt als Fallback fuer Build-Validierung erhalten, wird aber nicht als regulaerer Arbeitsweg genutzt.

Arduino IDE bleibt als dokumentierter Fallback fuer folgende Szenarien:

- schnelle Einzeltests ohne PlatformIO-Projekt
- Referenz fuer Bibliotheken, die zuerst ueber den Arduino Library Manager geprueft wurden
- Rueckfalloption falls PlatformIO-Build-Probleme auftreten

## Aktueller Stand

- Arduino IDE ist auf Windows installiert
- ESP32 Board Manager (Espressif Core): Stand noch nicht geprueft
- Bisher: Standard-Blink-Test geflasht, aktueller Zustand der Boards unbekannt
- Kein vollstaendiger Bench-Test ueber Arduino IDE durchgefuehrt

## Board Manager URL fuer Espressif ESP32 (Fallback-Referenz)

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Nach dem Eintragen im Board Manager: Espressif ESP32-Core installieren.
Fuer den `ESP32-S3-WROOM-1-N16R8` das Board `ESP32S3 Dev Module` waehlen.

## Inhalt (Fallback-Pfad)

- Board Manager URL und Grundkonfiguration fuer ESP32-S3
- Bibliotheksreferenz (dieselben Bibliotheken wie PlatformIO, nur anderer Installationsweg)
- Einfache Pruefpunkte fuer Einsatzbereitschaft als Fallback

## Regeln

- keine Versionsannahmen ohne Nachweis festschreiben
- reale Installationsstaende nur dokumentieren wenn durchgefuehrt
- Arduino IDE nicht als primaeren Entwicklungsweg behandeln

## Schnittstellen/Abhaengigkeiten

- bezieht sich auf `../../firmware/` und `../../docs/templates/`
- Primaerpfad: `../esp32_environment/README.md`
