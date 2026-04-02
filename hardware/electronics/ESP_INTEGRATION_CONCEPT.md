# ESP Integration Concept

Dieses Dokument beschreibt, wie die vorhandenen `ESP32-S3 DevKitC-1 N16R8` Boards in das Projekt eingebettet werden sollen.
Es trennt bewusst zwischen Bench-Startphase, wearablem Zwischenstand und spaeterem Dauerbetrieb.

## Ziel

- Sender- und Receiver-Elektronik klar voneinander trennen
- Breadboard nur dort nutzen, wo es sinnvoll und sicher ist
- den spaeteren Uebergang zu Lochraster und Gehaeuse frueh mitdenken
- die IMU-Halterungen modular halten und nicht mit schwerer Haupt-Elektronik ueberladen

## Aktuelle Boardbasis

Die aktuelle Dokumentationsbasis geht von `ESP32-S3 DevKitC-1 N16R8` aus.
Diese Boardwahl ist der Ausgangspunkt fuer Pinmapping, Debugkonzept und Gehaeuseplanung, soll aber die spaetere Austauschbarkeit anderer ESP32-S3- oder anderer Controllerboards nicht blockieren.

Bestaetigte Boarddetails (2026-03-22):

- Die vorhandenen Boards sind **Revision v1.0** — onboard RGB-LED liegt auf **`GPIO48`**.
- Modul: `ESP32-S3-WROOM-1-N16R8` mit 8 MB Octal-PSRAM (OPI) — GPIO35, GPIO36, GPIO37 sind intern belegt und nicht fuer User-I/O nutzbar.
- 4 Boards verfuegbar: 2 fuer Sender und Receiver, 2 als Reserve.
- Die onboard RGB-LED wird weiterhin nur als zusaetzliche Komfortanzeige behandelt und nicht als einziger Statuspfad fest eingeplant.

## Rollen der beiden ESP-Module

| Modul | Hauptaufgabe | Betriebsort |
| --- | --- | --- |
| Sender-ESP | IMUs, Mux, Flex-Sensor, erste Zustandsanzeigen, `ESP-NOW`-Sender | koerpergetragen |
| Receiver-ESP | `ESP-NOW`-Empfang, I2C-Weitergabe an Arduino (Slave 0x42), Warnsignale, Robotik-nahe Diagnose | am Roboter oder am Bench |

## Drei Integrationsstufen

### Stufe 1 - Offener Bench-Prototyp

- Sender und Receiver duerfen offen auf Breadboard oder Stecktraeger getestet werden.
- Fokus liegt auf Signalpfaden, Sensorerreichbarkeit, Pinbelegung und Basisdiagnose.
- Diese Stufe ist **nicht** fuer den koerpergetragenen oder dauerhaften Betrieb gedacht.

### Stufe 2 - Wearable Zwischenstand

- Sender-Elektronik wird von offenem Breadboard auf eine kleine Traegerplatine oder fruehe Lochraster-Loesung ueberfuehrt.
- Die Elektronik kommt in eine kleine Tasche, ein Pouch oder ein einfaches Traegergehaeuse.
- Der Sender wird an Schulter, Brustgurt oder Guertel getragen, **nicht** direkt auf den kleinen IMU-Gurten.

### Stufe 3 - Dauerbetriebsnaher Endstand

- Sender und Receiver gehen auf definierte Lochraster- oder spaeter PCB-nahe Traeger.
- Beide Seiten bekommen ein Gehaeuse- oder Tragerkonzept mit Zugentlastung, Steckkonzept und sichtbaren Debugpfaden.
- Die IMU-Module bleiben separat und austauschbar.

## Sender-Konzept

### Startphase

- `ESP32-S3 DevKitC-1 N16R8`
- `PCA9548A` oder anderer Sensorpfad fuer mehrere IMUs
- Flex-Sensor an ADC
- mindestens drei externe IMU-Status-LEDs
- optional spaeter weitere COMMS- oder Fehleranzeige

### Einbettung am Koerper

- kein offenes Breadboard direkt am Arm tragen
- erste tragbare Variante in weicher Tasche oder an kleinem Traegermodul
- bevorzugte Trageorte: Schultergurt, Brustbereich, Guertel oder Oberarmmodul mit separatem Elektroniktraeger
- IMU-Manschetten und Controller-Elektronik bleiben mechanisch getrennt

## Receiver-Konzept

### Startphase

- `ESP32-S3 DevKitC-1 N16R8` offen am Bench
- I2C-Verbindung zum Arduino (GPIO13 SDA, GPIO14 SCL → Arduino A4/A5, Slave 0x42)
- Link-, I2C- und Fehleranzeige sichtbar
- Hauptbuzzer auf der Roboter- oder Receiver-Seite

### Spaeterer Betrieb

- feste Montage am Robotersockel, Chassis oder einer separaten Elektroniktraegerplatte
- kurze und nachvollziehbare Verbindung zum Arduino
- Warnsignale und Statusanzeigen von aussen gut sichtbar bzw. hoerbar

## Stromversorgung der Arm-Seite

Fuer die Arm-Seite ist die offizielle Adeept-Produktbasis mitzudenken:

- die offizielle Produktseite nennt `2x18650` als Batteriekaufpunkt
- dieselbe Produktseite erlaubt alternativ Versorgung ueber `Micro USB`
- die gepruefte offizielle Support-Aussage von Adeept warnt jedoch, dass `Micro USB` bei Mehrservo-Betrieb zu wenig Strom liefern kann
- das importierte offizielle V4.0-Paket verankert `18650` zusaetzlich ueber `Read me first`, `Packing List`, Stock-Test und Lernmodus

Projektfolgerung:

- `Micro USB` wird nicht als automatisch belastbare Vollbewegungsversorgung fuer mehrere Servos behandelt
- vor echten Bewegungs- und Belastungstests muss ein dokumentierter Strompfad festgelegt werden
- als aktuelle Beschaffungsbasis gelten `4x Molicel INR-18650-M35A` ohne Loetfahne plus `1x XTAR VC4SL`
- reale Batteriefach-Passung, Beschaffung und Bewegungsfreigabe muessen vor dem modifizierten Echtbetrieb weiterhin bestaetigt werden
- Detailregeln und Beschaffungskriterien werden in `POWER_SUPPLY_CONCEPT.md` weitergefuehrt

## 3D-Gehaeuse- und Traegerkonzept

### Sender-Gehaeuse

Das dauerbetriebsnahe Sender-Gehaeuse soll:

- das DevKit plus kleine Traeger- oder Lochrasterplatine aufnehmen
- eine klar definierte USB-Oeffnung besitzen
- Reset- und Boot-Zugang nicht dauerhaft verbauen
- Kabelausgaenge mit Zugentlastung vorsehen
- die Antennenseite frei halten
- auf einen Gurt, eine Tasche oder ein textiles Traegersystem aufsetzbar sein

### Receiver-Gehaeuse

Das dauerbetriebsnahe Receiver-Gehaeuse soll:

- das DevKit, die Statusanzeigen und den Buzzer aufnehmen
- oeffenbare Wartung erlauben
- I2C-, Strom- und Debugzugang geordnet herausfuehren
- an Sockel, Grundplatte oder Tragrahmen des Roboterarms befestigbar sein
- die Antennenzone nicht mit Metall oder dicken Bauteilen abschatten

## Einfache Systemskizze

```text
Koerperseite

  IMU-Module ----\
  Flex-Sensor ---->  Sender-ESP in Pouch / Gehaeuse  )) ESP-NOW ))
                   (nicht direkt auf IMU-Gurt)

Roboterseite

  (( ESP-NOW ))  Receiver-ESP ---- I2C (GPIO13/14) ---- Arduino (A4/A5, 0x42) ---- Servos
                      |
                 LEDs + Buzzer
```

## Verbindliche Projektentscheidungen aus diesem Konzept

- Breadboard bleibt auf die offene Bench-Phase begrenzt.
- Der erste tragbare Sender darf provisorisch sein, soll aber schnell auf eine robustere Traegerloesung wechseln.
- IMU-Halterung und groessere Elektronik werden bewusst getrennt gehalten.
- Debug und Warnung werden verteilt gedacht: segmentnahe LEDs am Sender, Hauptwarnung ueber Buzzer am Receiver.

## Offene Punkte

- genaue Abmessungen des Sender-Gehaeuses erst nach Vermessung des realen Boards und der Zusatzplatinen festlegen
- dritten IMU auf dem bestehenden Mux-Pfad als naechste Segmentquelle bench-validieren
- reale Kabelfuehrung und Stecker erst nach erstem Bench-Aufbau festlegen
- offiziellen Stock-Strompfad des Adeept-Arms gegen den spaeteren Projekt-Strompfad sauber trennen
- Batteriekauf und reale Stromversorgungsentscheidung fuer Mehrservo-Bewegung dokumentieren

*Geschlossen: Boardrevision ist v1.0, onboard RGB-LED auf GPIO48, GPIO35/36/37 intern belegt (bestätigt 2026-03-22).*

## Recherchequellen

- [ESP32-S3-DevKitC-1 User Guide v1.1](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.1.html) fuer Boardlayout, Revision `v1.1` und onboard RGB-LED auf `GPIO38`.
- [ESP32-S3-DevKitC-1 User Guide v1.0](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.0.html) fuer die aeltere Boardvariante mit onboard RGB-LED auf `GPIO48`.
- [ESP32-S3 PCB Layout Design Guidelines](https://docs.espressif.com/projects/esp-hardware-design-guidelines/en/latest/esp32s3/pcb-layout-design.html) fuer Antennenfreiraum, Baseboard-Positionierung und Gehaeuseeinfluss auf RF.
- [Adafruit BNO055 Guide](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-bno055-absolute-orientation-sensor.pdf) fuer die dokumentierten Risiken von `BNO055` mit `ESP32-S3` und I2C-Multiplexern.
- [Adeept 5-DOF Robotic Arm Kit Compatible with Arduino IDE](https://www.adeept.com/adeept-5-dof-robotic-arm-kit-compatible-with-arduino-ide-programmable-diy-coding-stem-educational-5-axis-robot-arm-with-oled-display-processing-code_p0360.html) fuer den offiziellen Produkt- und Stromversorgungsrahmen des Arm-Kits.
- [Adeept Forum: Adeept 5-DOF Arduino ROBOT ARM](https://www.adeept.com/forum/thread-1009-1-1.html) fuer die offizielle Support-Aussage zum Strombedarf der Servos bei `Micro USB`-Versorgung.
