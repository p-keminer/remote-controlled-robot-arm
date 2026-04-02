# Prototype Breadboard Plan

Dieses Dokument beschreibt den fruehen offenen Elektronikpfad fuer Labor- und Bench-Tests.
Es ist kein Freifahrtschein fuer einen dauerhaften oder koerpergetragenen Breadboard-Betrieb.

## Ziel

- Sensor-, Funk-, ADC- und I2C-Pfade schnell testbar machen
- eine erste Pinbelegung pruefen
- Fehler sichtbar machen, bevor Lochraster oder Gehaeuse festgelegt werden

## Grundregel

`Breadboard = Bench-Prototyp`.

Fuer einen koerpergetragenen Zwischenstand ist ein offenes Breadboard nur als sehr kurzer Uebergang akzeptabel.
Sobald der Sender getragen wird, soll die Haupt-Elektronik auf eine kleine Traegerplatine, Lochrasterloesung oder ein vergleichbar robusteres Zwischenmodul wechseln.

## Sender-Benchaufbau

Der erste Sender-Benchaufbau soll enthalten:

- `ESP32-S3 DevKitC-1 N16R8`
- `PCA9548A` I2C-Mux (0x70) mit drei BNO055 auf Kanaelen 0/1/2
- Flex-Sensor mit Spannungsteiler 10kOhm an ADC (GPIO1)
- LED Gruen (GPIO4, 100 Ohm) — Hand/Wrist S2
- LED Gelb (GPIO5, 100 Ohm) — Unterarm S1
- LED Rot (GPIO6, 100 Ohm) — Oberarm S0
- LED Blau (GPIO7, 100 Ohm) — COMMS
- LED Weiss (GPIO10, 100 Ohm) — FAULT
- serielle Log-Ausgabe ueber USB-C

## Receiver-Benchaufbau

Der erste Receiver-Benchaufbau soll enthalten:

- `ESP32-S3 DevKitC-1 N16R8`
- I2C-Verbindung zum Arduino (GPIO13 SDA, GPIO14 SCL → Arduino A4/A5, Slave 0x42) — bench-validiert
- LED Gruen (GPIO4, 100 Ohm) — LINK
- LED Blau (GPIO5, 100 Ohm) — I2C (leuchtet bei I2C-Aktivitaet)
- LED Gelb (GPIO6, 100 Ohm) — FAULT
- einfachen Zugriff auf USB, Reset und Flashing

## Was auf Breadboard erlaubt ist

- offene Steckverkabelung fuer Labor- und Tischtests
- Dupont-Kabel fuer kurze Teststrecken
- sichtbare Debug-LEDs und provisorischer Buzzer
- schnelles Umbauen fuer Pin- und Sensorversuche

## Was auf Breadboard nicht der Zielzustand ist

- koerpergetragener Dauerbetrieb
- frei baumelnde Sensor- und Stromverkabelung
- finaler Robotik-Einbau
- finale Stromversorgung oder finale Safety-Freigabe

## Praktische Aufbauhinweise

- zuerst Einzelsensor mit direkter I2C-Verbindung pruefen, erst danach Multiplexerpfad erweitern
- Sender und Receiver getrennt aufbauen und einzeln validieren
- Kabel fuer IMUs und Flex-Sensor entlasten, auch schon am Bench
- fuer bewegte Tests die Elektronik nicht offen am Arm fuehren, sondern spaetestens dann in Pouch oder Traeger legen
- Debug und Sichtpruefung wichtiger als kompakte Bauform

## Einfache Bench-Skizze

```text
Sender-Bench (Stand 2026-03-26, bench-validiert)

  BNO055 #0 (Oberarm) ──┐
  BNO055 #1 (Unterarm) ─┼── PCA9548A ── ESP32-S3 ── LED Rot    (GPIO6, Oberarm)
  BNO055 #2 (Hand) ─────┘       │                   LED Gelb   (GPIO5, Unterarm)
                                 │                   LED Gruen  (GPIO4, Hand)
  Flex-Sensor ── 10k ── GPIO1 ──┘                   LED Blau   (GPIO7, COMMS)
                                                     LED Weiss  (GPIO10, FAULT)
                                                          │
                                                       USB-C / Logs

Receiver-Bench (Stand 2026-04-02, bench-validiert)

  ESP-NOW )) ESP32-S3 ──── I2C (GPIO13/14, 50Hz) ──── Arduino (A4/A5, Slave 0x42)
                 │                                          ├── D9  Base      12-139°
                 │                                          ├── D11 Shoulder  35-142°
                 │                                          ├── D3  Elbow     80-175°
                 │                                          ├── D5  Wrist      5-177°
                 │                                          └── D6  Gripper   32-126°
          LED Gruen  (GPIO4, LINK)
          LED Blau   (GPIO5, I2C-Aktivitaet)
          LED Gelb   (GPIO6, FAULT)
                 │
              USB-C / Logs
```

## Uebergang aus dem Breadboard heraus

Der Breadboardpfad gilt als erfolgreich vorbereitet, wenn:

- Einzelsensor, Mux-Pfad, Flex-Sensor, `ESP-NOW` und I2C jeweils separat nachvollziehbar funktionieren
- die bevorzugte GPIO-Belegung keine unmittelbaren Konflikte zeigt
- der Buzzer keine unerwuenschten Einschalttoene durch rohe Bootzustandswechsel produziert
- klar ist, welche Stecker, Kabellaengen und Trageorte in den Lochrasterpfad uebernommen werden

## Projektentscheidung hinter der Breadboard-Grenze

Die harte Grenze gegen koerpergetragenen Breadboard-Dauerbetrieb ist eine Projektentscheidung.
Sie wird durch zwei Dinge gestuetzt:

- Wearable-Systeme brauchen auch mechanisch stabile und wiederholbare Befestigung.
- Funk- und Sensorpfade sollen nicht durch lose, zugempfindliche Testverkabelung unnoetig instabil werden.

## Recherchequellen

- [ESP32-S3-DevKitC-1 User Guide v1.1](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.1.html) fuer Boardanschluesse, Grundlayout und zugaengliche Schnittstellen.
- [ESP32-S3 PCB Layout Design Guidelines](https://docs.espressif.com/projects/esp-hardware-design-guidelines/en/latest/esp32s3/pcb-layout-design.html) fuer Antennenfreiraum und spaetere Baseboard-Positionierung, die schon im Breadboardpfad mitgedacht werden sollte.
- [Tracking Upper Limb Motion via Wearable Solutions](https://pmc.ncbi.nlm.nih.gov/articles/PMC11704657/) fuer den Wearable-Bezug und die Notwendigkeit stabiler, koerpernaher Trageformen statt losem Testaufbau.
- [Adafruit BNO055 Guide](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-bno055-absolute-orientation-sensor.pdf) fuer die dokumentierten Randbedingungen von `BNO055` mit `ESP32-S3` und I2C-Multiplexern.
