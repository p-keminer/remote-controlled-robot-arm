# Debug Signals And Buzzer

Dieses Dokument beschreibt die geplanten sicht- und hoerbaren Zustandsanzeigen fuer Sender und Receiver.
Die Kombination aus LEDs und Buzzer soll Debug, Bringup und spaetere Safety-Reaktionen besser nachvollziehbar machen.

## Ziel

- Sensoraktivitaet am Sender direkt sichtbar machen
- Funk-, UART- und Fehlerstatus am Receiver klar zeigen
- Warnungen nicht nur in Logs verstecken
- Safety-Reaktionen akustisch und optisch erkennbar machen

## Grundsaetze

- der Buzzer ist ein **Ergaenzungskanal**, kein Ersatz fuer sichtbare Zustandsanzeige oder sichere Stopplogik
- die wichtigste akustische Warnung sitzt auf der Receiver- bzw. Roboterseite
- die drei IMU-Status-LEDs gehoeren primaer auf die Sender-Seite
- die onboard RGB-LED ist trotz bestaetigter Boardrevision nur Zusatzkomfort und kein alleiniger Primaerpfad

## Sender-Signale

### Minimalkonzept

- `LED Oberarm` zeigt, dass gerade der Oberarm-Sensorpfad gelesen, geprueft oder im Fokus der Diagnose ist
- `LED Unterarm` zeigt dasselbe fuer den Unterarm
- `LED Hand/Wrist` zeigt dasselbe fuer den Hand-/Wrist-Pfad

### Sinnvolle Erweiterungen

- `COMMS LED` blinkt bei erfolgreichen `ESP-NOW`-Sendezyklen
- `FAULT LED` leuchtet oder blinkt bei IMU-, Mux-, Timeout- oder Plausibilitaetsfehlern
- onboard RGB nur als zusammenfassender Zustand:
  - gruen = Grundbetrieb okay
  - gelb = degradiert / Warnung
  - rot = Fehler / Stop / Neutralmodus

## Receiver-Signale

### Minimalkonzept

- `LINK LED` fuer gueltige Funkframes
- `UART LED` fuer gueltige Weitergabe an Arduino
- `FAULT LED` fuer Timeout, Integritaetsfehler oder Safety-Fallback

### Hauptbuzzer

Der Hauptbuzzer sitzt auf der Receiver- oder Robotikseite, weil dort die letzte Sicherheitskette vor der Servoebene sichtbar wird.

## Vorgeschlagene Tonlogik

| Ereignis | Tonidee | Zweck |
| --- | --- | --- |
| System bereit | 1 kurzer Ton | Grundbereitschaft bestaetigen |
| Kalibrierung / Start abgeschlossen | 2 kurze Toene | erfolgreicher Uebergang in den Betriebsmodus |
| Funk fehlt / Daten fehlen | langsamer periodischer Ton | Kommunikationsverlust frueh hoerbar machen |
| Safety-Fallback / Neutralmodus | schneller Warnrhythmus | kritischen Zustand nicht nur optisch melden |
| ungueltige Datenkette / harte Warnung | kurzer scharfer Warnton | unmittelbare Aufmerksamkeit erzeugen |

## Elektrische Leitlinien

- jede LED bekommt ihren Vorwiderstand und wird als klare Einzelfunktion behandelt
- fuer den Buzzer ist ein Treiber- oder Schaltpfad mit sicherem Default-Aus-Zustand vorzuziehen
- fuer passive Buzzer oder differenziertere Tonausgabe ist PWM ueber `LEDC` der bevorzugte Pfad
- fuer aktive Buzzer koennen auch einfache Warnmuster ausreichen, wenn sie elektrisch sauber eingebunden sind

## Warum der Buzzer nicht der einzige Safety-Kanal sein darf

- akustische Signale koennen ueberhoert oder spaeter deaktiviert werden
- die Servoebene braucht weiterhin echte Neutral-, Stop- und Watchdog-Logik
- sichtbare Fehleranzeigen und dokumentierte Fehlerreaktionen bleiben Pflicht

## Gehaeusebezug

- Sender-Gehaeuse braucht mindestens gut sichtbare LED-Fenster oder Lichtleiter
- Receiver-Gehaeuse braucht sichtbare Statusanzeigen und eine Buzzer-Oeffnung
- akustische und optische Signale duerfen nicht versehentlich im Gehaeuse "eingemauert" werden

## Projektentscheidung fuer v1

- drei IMU-Status-LEDs am Controller sind gesetzt
- Buzzer entfaellt vorerst — LEDs und RGB reichen fuer Debugging
- **LED-Schema invertiert:** aus = OK, an = Problem (weniger Strom, klare Fehlererkennung)
- **RGB-LED auf GPIO48** ist primaerer FAULT-Indikator auf allen drei ESPs (Controller, Receiver, Bridge)
- externe LEDs sind weiterhin primaer fuer kanalspezifische Statusanzeige (WiFi, ESP-NOW, MQTT, Kalibrierung)
- Bridge hat zusaetzlich weisse LED fuer MQTT-Status

### Aktuelle LED-Belegung (Stand 2026-03-26)

**Controller:** GPIO4 Gruen(S2), GPIO5 Gelb(S1), GPIO6 Gelb(S0), GPIO7 Blau(COMMS), GPIO48 RGB(FAULT)
**Receiver:** GPIO4 Gruen(UART), GPIO5 Blau(ESP-NOW), GPIO48 RGB(FAULT)
**Bridge:** GPIO4 Gruen(WiFi), GPIO5 Blau(ESP-NOW), GPIO7 Weiss(MQTT), GPIO48 RGB(FAULT)

## Recherchequellen

- [ESP32-S3-DevKitC-1 User Guide v1.1](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.1.html) fuer die vorhandenen Boardfunktionen und die onboard RGB-LED der Revision `v1.1`.
- [ESP32-S3-DevKitC-1 User Guide v1.0](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.0.html) fuer die aeltere RGB-LED-Zuordnung auf `GPIO48`.
- [ESP32-S3 LEDC Documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/peripherals/ledc.html) fuer PWM-basierte Signalausgabe, wie sie fuer einen passiven Buzzer sinnvoll ist.
- [ESP32-S3 GPIO & RTC GPIO](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/peripherals/gpio.html) fuer die GPIO-Grundlagen, auf denen LED- und Buzzerpfade aufgebaut werden.
