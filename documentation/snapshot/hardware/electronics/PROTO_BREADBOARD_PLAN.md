# Prototype Breadboard Plan

Dieses Dokument beschreibt den fruehen offenen Elektronikpfad fuer Labor- und Bench-Tests.
Es ist kein Freifahrtschein fuer einen dauerhaften oder koerpergetragenen Breadboard-Betrieb.

## Ziel

- Sensor-, Funk-, ADC- und UART-Pfade schnell testbar machen
- eine erste Pinbelegung pruefen
- Fehler sichtbar machen, bevor Lochraster oder Gehaeuse festgelegt werden

## Grundregel

`Breadboard = Bench-Prototyp`.

Fuer einen koerpergetragenen Zwischenstand ist ein offenes Breadboard nur als sehr kurzer Uebergang akzeptabel.
Sobald der Sender getragen wird, soll die Haupt-Elektronik auf eine kleine Traegerplatine, Lochrasterloesung oder ein vergleichbar robusteres Zwischenmodul wechseln.

## Sender-Benchaufbau

Der erste Sender-Benchaufbau soll enthalten:

- `ESP32-S3 DevKitC-1 N16R8`
- `PCA9548A` oder den aktuell geplanten Mehr-IMU-Pfad
- mindestens einen einzelnen IMU-Testpfad vor dem Mehr-IMU-Test
- Flex-Sensor mit Spannungsteiler an ADC
- drei externe Status-LEDs fuer Oberarm, Unterarm und Hand/Wrist
- optionale vierte LED fuer COMMS oder Fehler
- serielle Log-Ausgabe ueber den Standard-Debugpfad

## Receiver-Benchaufbau

Der erste Receiver-Benchaufbau soll enthalten:

- `ESP32-S3 DevKitC-1 N16R8`
- UART-Testpfad zum Arduino
- mindestens drei sichtbare Status-LEDs fuer Funk, UART und Fehler
- Hauptbuzzer fuer Warn- und Safety-Zustaende
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
Sender-Bench

  IMU(s) ---- PCA9548A ---- ESP32-S3 ---- LED 1
                                 |        LED 2
  Flex-Sensor -------------------+        LED 3
                                 |
                              USB / Logs

Receiver-Bench

  ESP-NOW )) ESP32-S3 ---- UART ---- Arduino
                 |            |
               LEDs         Buzzer
```

## Uebergang aus dem Breadboard heraus

Der Breadboardpfad gilt als erfolgreich vorbereitet, wenn:

- Einzelsensor, Mux-Pfad, Flex-Sensor, `ESP-NOW` und UART jeweils separat nachvollziehbar funktionieren
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
