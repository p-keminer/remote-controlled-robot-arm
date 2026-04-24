# Debug Signals

Dieses Dokument beschreibt die sichtbaren Statusanzeigen des aktuellen Prototyps.

## Grundsaetze

- LEDs und RGB sind die aktiven Hardware-Statuspfade.
- Ein Buzzer ist keine aktive Projektkomponente.
- Das LED-Schema bleibt invertiert: aus = OK, an = Problem.

## Controller

- GPIO4 = Oberarm / S2
- GPIO5 = Unterarm / S1
- GPIO6 = Hand / S0
- GPIO7 = COMMS
- GPIO10 = FAULT
- GPIO48 = RGB fuer Fehler und Notaus

## Receiver

- GPIO4 = I2C
- GPIO5 = LINK
- GPIO48 = RGB

## Bridge

- GPIO4 = WiFi
- GPIO5 = ESP-NOW
- GPIO7 = MQTT
- GPIO48 = RGB

## Hinweis zur Greifer-Eingabe

Die Greifer-Eingabe kommt aktuell aus einem Potentiometer.
Fehleranzeigen beziehen sich daher auf die generische Greifer-Eingabe und nicht mehr auf eine fruehere Sensorspezialloesung.
