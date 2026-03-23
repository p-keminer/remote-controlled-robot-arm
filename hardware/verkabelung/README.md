# hardware/verkabelung

## Zweck

Dieser Ordner beschreibt die reale Verdrahtung des Systems.

## Aktueller Stand

Die Struktur ist vorbereitet, die tatsaechliche Verkabelungsdokumentation wird spaeter nachgetragen.
Dabei sollen originaler Stock-Aufbau und spaeterer Projekt-Umbau als getrennte Verdrahtungszustaende dokumentiert werden.
Der offizielle Stock-Test dokumentiert ausserdem sechs Servoports mit Nummerierung `1-6` in OLED-Richtung und warnt vor falscher Steckerorientierung.

## Inhalt

- Kabelwege zwischen ESP32, Multiplexer, IMUs, Flex-Sensor und Arduino
- Steckverbindungen, Pinbelegungen und Adapterhinweise
- Abweichungen zwischen Schaltplan und Aufbau
- Stock-Servoport-Reihenfolge des Adeept-Boards als Referenz

## Regeln

- reale Verkabelung immer von theoretischen Schaltplaenen abgrenzen
- pro Aufbauzustand klar benennen, welche Version dargestellt ist
- Stock-Nummerierung und Projektbezeichnung nicht vermischen; Abweichungen explizit markieren

## Schnittstellen/Abhaengigkeiten

- basiert auf `hardware/schaltplaene/`
- wird durch Fotos in `docs/photos/` und Notizen in `docs/session_notes/` ergaenzt

## Recherchequellen

- [Adeept V4 Package Review](../../official_downloads/ADEEPT_V4_PACKAGE_REVIEW.md) fuer die Einordnung der Stock-Servoport-Reihenfolge und der sechsten Test-/Reserveposition.
- [_5 Test Adeept Arm Drive Board, Servo and etc..pdf](../../official_downloads/extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Tutorials-pdf/_5%20Test%20Adeept%20Arm%20Drive%20Board,%20Servo%20and%20etc..pdf) fuer die Stock-Reihenfolge `1-6` in OLED-Richtung und den Hinweis, die Servo-Stecker nicht verkehrt herum anzuschliessen.
