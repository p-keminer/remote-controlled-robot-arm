# Adeept V4 Package Review

Dieses Dokument fasst die projektrelevanten Erkenntnisse aus dem offiziell verlinkten und lokal ausgewerteten Adeept-Download zusammen.
Es dient als Bruecke zwischen dem unveraenderten Herstellerpaket und den gepflegten Projektdokumenten.

## Zweck

- den offiziellen V4.0-Download des vorhandenen Adeept-Arms einordnen
- fuer das Projekt relevante Herstellerangaben zu Aufbau, Stromversorgung, Originalfunktionen und Boardannahmen extrahieren
- Widersprueche oder Spannungen zwischen Produktseite, Tutorial-PDFs und Originalcode sichtbar machen

## Gepruefter Herstellerstand

- offizieller Downloadpfad: `official_downloads/raw/README.md`
- entpackter Snapshot: `official_downloads/extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/`

## Relevante Inhalte im Download

### Offizielle Dokumentation

- `Read me first.pdf`
- `Adeept_Schematic_for_ArmBoard_V3.0.pdf`
- `Tutorials-pdf/_2 Packing List.pdf`
- `Tutorials-pdf/_3 Introduction of Adeept Arm Drive Board .pdf`
- `Tutorials-pdf/_5 Test Adeept Arm Drive Board, Servo and etc..pdf`
- `Tutorials-pdf/_6 Servo 90-degree adjustment.pdf`
- `Tutorials-pdf/5. Assembly of the Robotic Arm.pdf`
- `Tutorials-pdf/6. How to use GUI to control Robotic Arm.pdf`
- `Tutorials-pdf/7. You might as well try a potentiometer to control Robotic Arm.pdf`
- `Tutorials-pdf/8. How to let the Robotic Arm learn our control action.pdf`
- `Tutorials-pdf/9. How to use Processing IDE controls Robotic arm.pdf`

### Originalcode und Softwarepakete

- `Code/_5 Unpacking_test_code/Unpacking_test_code/Unpacking_test_code.ino`
- `Code/_6 Servo90/Servo90/Servo90.ino`
- `Code/7. Potentiometer_control_mode/Potentiometer_control/Potentiometer_control.ino`
- `Code/8. Learning/Learning/Learning.ino`
- `Code/9. Processing/AdeeptArmRobot/AdeeptArmRobot.ino`
- `Code/9. Processing/ProcessingArmRobot/ProcessingArmRobot.pde`
- `Software Package/Adeept driver/` mit CH341-Treiberpfad
- `Software Package/websocket/` mit Python-GUI-Dateien

## Projektrelevante Erkenntnisse

### 1. Die offizielle Herstellerbasis ist klar Arduino-UNO-zentriert

- Der offizielle V4.0-Stand behandelt das `Adeept Arm Drive Board` als Arduino-kompatibles Board.
- In den Tutorials wird fuer den Stock-Pfad `Arduino Uno` als Boardauswahl verwendet.
- Der offizielle Windows-Setup-Pfad nennt den CH341-Treiber im Herstellerpaket.

### 2. Die 5-vs-6-Servo-Frage ist jetzt deutlich klarer

Die offiziellen Quellen widersprechen sich nicht vollstaendig, sondern zeigen zwei Ebenen:

- Die Produktseite nennt `6x AD002 Servo`.
- Die `Packing List` im offiziellen V4.0-Download nennt `Servo x5`.
- Das offizielle `Unpacking_test_code`-Tutorial sagt, dass alle 6 steering gears an das Board angeschlossen werden sollen, ausdruecklich `including one spare steering gear`.
- `Unpacking_test_code.ino` testet sechs Servoports auf den Pins `9, 6, 5, 3, 11, 10`.
- `Servo90.ino`, `Potentiometer_control.ino`, `Learning.ino` und `AdeeptArmRobot.ino` arbeiten dagegen mit fuenf aktiven Laufzeitservos auf `9, 6, 5, 3, 11`.

Arbeitsinterpretation fuer dieses Projekt:

- der Arm selbst wird offiziell als `5-DOF` mit fuenf aktiven Betriebsservos behandelt
- der sechste Servo taucht im Herstellerstand als Reserve- bzw. Testservo auf
- die sechste Boardposition `D10/servo6` ist deshalb als Stock-Test- oder Ersatzpfad mitzudenken, nicht automatisch als zusaetzliche Projektachse

Diese Interpretation ist eine Schlussfolgerung aus den offiziellen Quellen und muss beim realen Unpacking des vorhandenen Kits bestaetigt werden.

### 3. Die Stromversorgung ist im offiziellen Material klar batterielastig

- `Read me first.pdf` fordert den Kauf formaler und qualifizierter `18650`-Lithiumzellen.
- Die `Packing List` nennt `18650 Battery x2` als selbst zu beschaffende Teile.
- Das Stock-Test-Tutorial laesst die Basis inklusive Batteriefach aufbauen und zuerst das Batteriekabel an das Driver Board anschliessen.
- Das Lern-Tutorial nennt fuer den Lernbetrieb ausdruecklich eine voll geladene Batterie.

Projektfolgerung:

- `Micro USB` ist im Herstellerstand primaer Upload- und Monitoring-Pfad
- Batterieversorgung ist nicht nur optionaler Komfort, sondern im offiziellen Paket- und Tutorialfluss deutlich mitgedacht
- fuer reale Bewegungs- und Lasttests darf der Arm nicht auf `USB-only` reduziert geplant werden

### 4. Der Herstellerstand liefert verbindliche Stock-Test-Schritte

- Vor dem mechanischen Zusammenbau ist ein offizieller `Servo90`-Schritt vorgesehen.
- Fuer den Board-/Servo-/OLED-/Poti-Check existiert ein eigener `Unpacking_test_code`-Pfad.
- Die Reihenfolge `Servo90 -> Stock-Aufbau -> Unpacking-Test -> weitere Originalmodi` ist dadurch sauber nachvollziehbar.

### 5. Die Originalfunktionen des Kits sind breiter als nur einfache Servosteuerung

Der offizielle Download enthaelt nicht nur Aufbau- und Testmaterial, sondern auch Originalfunktionen, die fuer die Projektabgrenzung wichtig sind:

- Potentiometer-Steuerung auf dem Driver Board
- OLED-Ausgabe
- EEPROM-Learning bzw. Action Memory
- GUI/Python-Steuerung
- Processing-basierte PC-Steuerung
- Imitation, Schreiben und Zeichnen

## Projektfolgen

- `hardware/ADEEPT_ARM_PRODUCT_BASELINE.md` wird auf `5 aktive Servos + 1 Reserve/Testservo` als Arbeitsstand ausgerichtet
- `firmware/arduino_arm/README.md` kann die offizielle Stock-Pinbasis jetzt sauber auf `D9, D6, D5, D3, D11` festziehen
- `hardware/electronics/POWER_SUPPLY_CONCEPT.md` behandelt `2x18650` als stock-kompatiblen Energiepfad und `Micro USB` nicht als Vollbetriebsannahme
- `preparation/arduino_ide_setup/README.md` kann den Herstellerpfad `Arduino Uno + CH341` als offizielle Stock-Basis dokumentieren, ohne die Projekt-Hauptumgebung zu aendern

## Offene Punkte trotz offizieller Sichtung

- realen Lieferumfang des vorhandenen Kits gegen den Hersteller-Snapshot bestaetigen
- reale Rolle des sechsten Servos beim echten Unpacking pruefen
- Stromaufnahme des aufgebauten Arms fuer Kauf- und Dauerbetriebsentscheidung messen
- festlegen, welche Originalfunktionen des Adeept-Kits spaeter ersetzt oder bewusst beibehalten werden

## Recherchequellen

- [Adeept Robotic Arm UNO](https://www.adeept.com/robotic-arm-uno_p0118.html) fuer den direkten Produkt- und Kaufpfad des Herstellerkits.
- [Adeept Learn Detail 64](https://www.adeept.com/learn/detail-64.html) fuer den offiziellen Download- und Lernpfad des Herstellerpakets.
- [Read me first.pdf](./extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Read%20me%20first.pdf) fuer Herstellerhinweise zu `18650`-Pflicht, Umgebungsgrenzen und dem Verbot, den Arm im Betrieb mit der Hand zu zwingen.
- [_2 Packing List.pdf](./extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Tutorials-pdf/_2%20Packing%20List.pdf) fuer `Servo x5`, `18650x2 Battery Holder x1`, `USB Cable x1` und `18650 Battery x2` als Selbstbeschaffung.
- [_3 Introduction of Adeept Arm Drive Board .pdf](./extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Tutorials-pdf/_3%20Introduction%20of%20Adeept%20Arm%20Drive%20Board%20.pdf) fuer `Vin (6-24V)`, `Micro USB` und die Potentiometerkanaele `A0, A1, A2, A3, A6`.
- [_5 Test Adeept Arm Drive Board, Servo and etc..pdf](./extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Tutorials-pdf/_5%20Test%20Adeept%20Arm%20Drive%20Board,%20Servo%20and%20etc..pdf) fuer den Stock-Testfluss, `including one spare steering gear`, die 6 Servoports und die `Arduino Uno`-Boardauswahl.
- [_6 Servo 90-degree adjustment.pdf](./extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Tutorials-pdf/_6%20Servo%2090-degree%20adjustment.pdf) fuer den offiziellen Vorab-Schritt zur 90-Grad-Ausrichtung der fuenf Betriebsservos.
- [Unpacking_test_code.ino](./extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Code/_5%20Unpacking_test_code/Unpacking_test_code/Unpacking_test_code.ino) fuer die sechs getesteten Servoports `9, 6, 5, 3, 11, 10`.
- [Servo90.ino](./extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Code/_6%20Servo90/Servo90/Servo90.ino) fuer die offizielle 5-Servo-90-Grad-Basis auf `9, 6, 5, 3, 11`.
- [AdeeptArmRobot.ino](./extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Code/9.%20Processing/AdeeptArmRobot/AdeeptArmRobot.ino) fuer die offizielle 5-Servo-Laufzeitbasis des Processing-Steuerpfads.
