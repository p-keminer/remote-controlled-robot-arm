# firmware

## Zweck

Dieser Ordner enthaelt die gesamte Laufzeitlogik fuer Controller, Receiver und Servoausfuehrung.

## Aktueller Stand

Die Firmwarestruktur ist dokumentiert, die eigentliche Implementierung steht noch am Anfang.
Die Verantwortungsgrenzen zwischen Sensorik, Funkbruecke und Servoebene sind bereits festgelegt.

## Inhalt

- `esp32_controller/` fuer Wearable-Sensorik, Vorverarbeitung und Funksendung
- `esp32_receiver/` fuer Funkempfang, Validierung und UART-Uebergabe
- `arduino_arm/` fuer direkte Servoansteuerung, Limits und Rampen

## Regeln

- keine Sensor-, Funk- oder Servologik stillschweigend zwischen den Teilbereichen vermischen
- Protokollaenderungen immer in `COMMUNICATION_FRAMEWORK.md` dokumentieren
- Limits und Neutralverhalten immer auch mit `SAFETY_FRAMEWORK.md` und `calibration/` abstimmen

## Schnittstellen/Abhaengigkeiten

- bezieht Kommunikationsregeln aus `../COMMUNICATION_FRAMEWORK.md`
- bezieht Kalibrier- und Grenzwerte aus `../CALIBRATION_FRAMEWORK.md` und `../calibration/`
- bezieht Sicherheitsanforderungen aus `../SAFETY_FRAMEWORK.md`
