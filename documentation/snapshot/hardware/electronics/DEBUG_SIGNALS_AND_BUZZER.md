# Debug Signals

Dieses Dokument beschreibt die sichtbaren Zustandsanzeigen fuer Sender, Receiver und Bridge.
LEDs und die onboard RGB-LED sind die einzigen Statuspfade — ein Buzzer wurde als Projektentscheidung gestrichen (LEDs und RGB reichen fuer Debugging).

## Ziel

- Sensoraktivitaet am Sender direkt sichtbar machen
- Funk-, I2C- und Fehlerstatus am Receiver klar zeigen
- Warnungen nicht nur in Logs verstecken
- Safety-Reaktionen optisch erkennbar machen (RGB orange blinkend bei Notaus)

## Grundsaetze

- die drei IMU-Status-LEDs gehoeren primaer auf die Sender-Seite
- die onboard RGB-LED ist trotz bestaetigter Boardrevision nur Zusatzkomfort und kein alleiniger Primaerpfad
- **LED-Schema invertiert:** aus = OK, an = Problem (weniger Strom, klare Fehlererkennung)
- **RGB-LED auf GPIO48** ist primaerer FAULT-/Notaus-Indikator auf allen drei ESPs

## Sender-Signale (Controller)

### Minimalkonzept

- `LED Oberarm` (GPIO6, Rot) zeigt, dass der Oberarm-Sensorpfad ein Problem hat
- `LED Unterarm` (GPIO5, Gelb) zeigt dasselbe fuer den Unterarm
- `LED Hand/Wrist` (GPIO4, Gruen) zeigt dasselbe fuer den Hand-/Wrist-Pfad

### Erweiterungen

- `COMMS LED` (GPIO7, Blau) leuchtet wenn ESP-NOW Send fehlschlaegt
- `FAULT LED` (GPIO10, Weiss) leuchtet bei IMU-, Mux-, Timeout- oder Plausibilitaetsfehlern
- onboard RGB (GPIO48): rot blinkend bei Sensorausfall/Flex-Fehler, orange blinkend bei Notaus, aus wenn OK
- Notaus-Toggle-Button auf GPIO21 (interner Pull-Up, 50ms Entprellung)

## Receiver-Signale

- `LINK LED` (GPIO4, Gruen) fuer gueltige Funkframes
- `I2C LED` (GPIO5, Blau) fuer gueltige Weitergabe an Arduino
- `FAULT LED` (GPIO6, Gelb) fuer Timeout, Integritaetsfehler oder Safety-Fallback
- onboard RGB (GPIO48): orange blinkend bei Notaus, rot bei Fehler, aus wenn OK

## Bridge-Signale

- `WiFi LED` (GPIO4, Gruen) fuer WiFi-Verbindungsstatus
- `ESP-NOW LED` (GPIO5, Blau) fuer ESP-NOW-Empfang
- `MQTT LED` (GPIO7, Weiss) fuer MQTT-Verbindungsstatus
- onboard RGB (GPIO48): orange blinkend bei Notaus, rot bei Fehler, aus wenn OK

## Aktuelle LED-Belegung (Stand 2026-04-02)

**Controller:** GPIO4 Gruen(S2), GPIO5 Gelb(S1), GPIO6 Rot(S0), GPIO7 Blau(COMMS), GPIO10 Weiss(FAULT), GPIO21 Notaus-Button, GPIO48 RGB(FAULT/Notaus)
**Receiver:** GPIO4 Gruen(LINK), GPIO5 Blau(I2C), GPIO48 RGB(FAULT/Notaus)
**Bridge:** GPIO4 Gruen(WiFi), GPIO5 Blau(ESP-NOW), GPIO7 Weiss(MQTT), GPIO48 RGB(FAULT/Notaus)

## Elektrische Leitlinien

- jede LED bekommt ihren Vorwiderstand (100 Ohm) und wird als klare Einzelfunktion behandelt
- die Servoebene braucht weiterhin echte Neutral-, Stop- und Watchdog-Logik — LEDs sind nur Anzeige, kein Ersatz
- sichtbare Fehleranzeigen und dokumentierte Fehlerreaktionen bleiben Pflicht

## Gehaeusebezug

- Sender-Gehaeuse braucht mindestens gut sichtbare LED-Fenster oder Lichtleiter
- Receiver-Gehaeuse braucht sichtbare Statusanzeigen
- optische Signale duerfen nicht versehentlich im Gehaeuse "eingemauert" werden

## Recherchequellen

- [ESP32-S3-DevKitC-1 User Guide v1.1](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.1.html) fuer die vorhandenen Boardfunktionen und die onboard RGB-LED der Revision `v1.1`.
- [ESP32-S3-DevKitC-1 User Guide v1.0](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.0.html) fuer die aeltere RGB-LED-Zuordnung auf `GPIO48`.
- [ESP32-S3 GPIO & RTC GPIO](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/peripherals/gpio.html) fuer die GPIO-Grundlagen, auf denen LED-Pfade aufgebaut werden.
