# firmware/esp32_receiver

## Zweck

Dieser Ordner ist fuer die Receiver-Firmware des zweiten ESP32 reserviert.

## Aktueller Stand

Der Bereich ist strukturell vorbereitet.
Der Receiver ist als validierende Bruecke zwischen Funkstrecke und Arduino definiert.

## Inhalt

- spaetere ESP-NOW-Empfangslogik
- Paketvalidierung und Zeitueberwachung
- UART-Weitergabe an das Adeept-Arduino-Board
- Fehlerreaktion bei ungueltigen oder fehlenden Daten

## Regeln

- der Receiver enthaelt keine direkte Servo-PWM
- unvalidierte Pakete duerfen nicht an den Arduino weitergereicht werden
- Timeout- und Watchdog-Annahmen muessen mit `SAFETY_FRAMEWORK.md` konsistent sein

## Schnittstellen/Abhaengigkeiten

- empfaengt Zielwerte aus `../esp32_controller/`
- uebergibt serielle Frames an `../arduino_arm/`
- orientiert sich an `../../COMMUNICATION_FRAMEWORK.md` und `../../SAFETY_FRAMEWORK.md`
