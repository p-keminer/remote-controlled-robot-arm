# Adeept Arm Product Baseline

Dieses Dokument haelt die produktbezogene Ausgangsbasis fuer den realen Roboterarm fest.
Es beschreibt, welches Adeept-Kit konkret als Hardware-Ursprung behandelt wird, welche offiziellen Produktangaben fuer das Projekt relevant sind und welche produktnahen Klaerungspunkte noch offen bleiben.

## Zweck

- den konkreten Roboterarm als feste Produktbasis dokumentieren
- die offizielle Package- und Stromversorgungsbasis sichtbar machen
- Stock-Zustand und spaeteren Umbau sauber voneinander trennen
- offene produktbezogene Klaerungspunkte an einer zentralen Stelle sammeln

## Produktidentitaet

- Produktfamilie: `Adeept 5-DOF Robotic Arm Kit`
- offizielle Produktbeschreibung: `Compatible with Arduino IDE`, `5 Axis Robot Arm`, `OLED Display`, `Processing Code`
- offizielle Tutorial-Ressource: `Adeept Robotic Arm Kit for Arduino - V4.0`
- vorhandene Kaufreferenz des Projektkits: Amazon-Bestellung unter `B085C2S1N7` laut Nutzerangabe, aktuell referenziert ueber [Amazon: Adeept 5-DOF Roboterarm-Kit](https://www.amazon.de/dp/B085C2S1N7?ref=ppx_yo2ov_dt_b_fed_asin_title)
- offizieller lokaler Hersteller-Snapshot im Repo: `official_downloads/` mit ZIP, Tutorial-PDFs, Originalcode und Schaltplan

## Offiziell dokumentierte Produktbasis

Aus den aktuell geprueften offiziellen Adeept-Quellen ergibt sich fuer dieses Projekt folgende Baseline:

- der Hauptcontroller des Originalkits ist das `Adeept Robotic Arm Driver Board`
- dieses Board ist laut Hersteller auf `ATMEGA328P` aufgebaut
- in der Arduino IDE soll fuer das Originalsystem `Arduino UNO R3` als Boardmodell gewaehlt werden
- das Kit wird mit `0.96'' OLED Display` beschrieben
- die offiziellen Produktangaben nennen `Manual Control`, `Remote Control` ueber Processing sowie weitere Originalmodi wie `Self-learning/Action memory`, `drawing` und `imitating`
- der importierte offizielle V4.0-Download enthaelt zusaetzlich Aufbau-, Test-, Servo90-, GUI-, Processing- und Lernunterlagen samt Originalcode

## Offiziell dokumentierter Lieferumfang

Die aktuell gepruefte offizielle Produktseite nennt unter anderem:

- `1 Set Structure Parts (Acrylic)`
- `1x Adeept Robotic Arm Driver Board`
- `1x 0.96'' OLED Display`
- `6x AD002 Servo`
- `1x Extension Cable for Servo`
- `1x Square Bearing Turntable`
- `1x Battery Holder`
- `1x Micro USB Cable`
- weitere Befestigungs- und Verdrahtungsteile

Der importierte offizielle V4.0-Download praezisiert den Lieferumfang an einer anderen Stelle:

- `Servo x5`
- `18650x2 Battery Holder x1`
- `USB Cable x1`
- `18650 Battery x2` als selbst zu beschaffende Teile

## Arbeitsstand zur 5-vs-6-Servo-Frage

Die geprueften offiziellen Quellen ergeben jetzt ein konsistenteres Bild:

- die Produktseite nennt `6x AD002 Servo`
- die `Packing List` des offiziellen V4.0-Downloads nennt `Servo x5`
- das offizielle Stock-Test-Tutorial fordert, alle `6 steering gears` anzuschliessen, ausdruecklich `including one spare steering gear`
- `Unpacking_test_code.ino` testet die sechs Boardports `9, 6, 5, 3, 11, 10`
- `Servo90.ino`, `Potentiometer_control.ino`, `Learning.ino` und `AdeeptArmRobot.ino` arbeiten mit fuenf aktiven Betriebsservos auf `9, 6, 5, 3, 11`

Arbeitsinterpretation fuer dieses Projekt:

- der offizielle Arm ist ein `5-DOF`-System mit fuenf aktiven Betriebsservos
- der sechste Servo ist im Herstellerstand als Reserve- bzw. Testservo mitzudenken
- die sechste Boardposition `D10/servo6` wird deshalb bis zum realen Unpacking nicht als zusaetzliche Projektachse behandelt

## Stromversorgung laut offizieller Adeept-Doku

Der aktuell gepruefte offizielle Produkttext sagt:

- Batterien sind nicht enthalten
- fuer Batteriebetrieb sollen `2x18650` beschafft werden
- alternativ kann der Arm laut Produktseite auch direkt ueber `Micro USB` ohne Batterien versorgt werden
- `Read me first.pdf` fordert ausdruecklich formale und qualifizierte `18650`-Lithiumzellen
- das Lern-Tutorial nennt fuer den entsprechenden Originalmodus eine voll geladene Batterie

Zusaetzlich gibt es eine offizielle Support-Aussage von Adeept:

- fuer Mehrservo-Bewegung kann reine `Micro USB`-Versorgung zu wenig Strom liefern
- laut Support fuehrt der hohe Startstrom der Servos dabei zu Spannungsabfall und Reset-Problemen
- Adeept empfiehlt in diesem Fall die Versorgung ueber `18650`-Batterien

## Verbindliche Projektfolgen aus dieser Baseline

- vor Umbauten wird ein dokumentierter Stock-Baseline-Test des Originalsystems eingeplant
- der offizielle Stock-Baseline-Test soll sich am Herstellerpfad `Servo90 -> Aufbau -> Unpacking_test_code` orientieren
- `Micro USB` gilt nicht als automatisch ausgeschlossene Versorgung, aber auch nicht als verlaesslich freigegebene Vollbewegungsversorgung fuer Mehrservo-Betrieb
- fuer reale Bewegungs- und Belastungstests muss ein dokumentierter Strompfad festgelegt werden
- Batteriekauf bzw. Akkuauswahl ist kein optionaler Randpunkt mehr, sondern ein eigenes Beschaffungs- und Konzeptthema
- den Arm im Betrieb per Hand zu zwingen wird als offizielle Herstellerwarnung behandelt und deshalb auch projektseitig als Safety-Regel uebernommen

## Aktuelle Projektentscheidungen zum Stock-Kit

### Welche Originalfunktionen bleiben erhalten?

Fuer den aktuellen Projektstand ist entschieden:

- die Potentiometer-Steuerung des Stock-Kits bleibt als Fallback erhalten
- das `0.96'' OLED Display` wird als lokale Anzeige des Stock-Systems uebernommen
- `Learning` bzw. `Action Memory` bleibt als gewollte Originalfunktion erhalten
- die Processing-basierte PC-Steuerung bleibt vorerst als Referenz- und Fallbackpfad dokumentiert
- spaeter soll die Processing-Steuerung durch ein eigenes Dashboard, eine moegliche Handy-App und einen eigens konstruierten Controller abgeloest werden
- zusaetzlich bleibt ein spaeterer Controller-Pfad mit z.B. Xbox-Controller als Zukunftsausbau vorgesehen

### Welche Stromversorgung ist fuer den ersten Projektstand vorgesehen?

Als dokumentierter Arbeitsstand fuer Beschaffung und spaetere Stock-/Projektversuche gilt:

- `4x Molicel INR-18650-M35A` ohne Loetfahne als Akkubasis
- `1x XTAR VC4SL` als Ladegeraet
- `Flat Top` wird als Startannahme bevorzugt; ein erhoehter Pluspol wird erst bei realem Batteriefachbedarf in Betracht gezogen

## Offene Klaerungspunkte

### Wie bestaetigt sich der Arbeitsstand `5 aktive + 1 Reserve/Testservo` am realen Kit?

Die offizielle Doku ist jetzt deutlich klarer, aber der reale Kit-Inhalt muss trotzdem noch bestaetigt werden:

- die tatsaechliche Servozahl beim Unpacking dokumentieren
- festhalten, ob ein separater Reserverservo physisch im Karton liegt
- beim Aufbau dokumentieren, welches Servo welcher Funktion zugeordnet ist
- erst danach in `firmware/arduino_arm/` und `calibration/servo_limits/` die reale Zuordnung final festziehen

### Welche Stromversorgung wird fuer das Projekt verbindlich?

Noch offen bleibt trotz Auswahl:

- ob `Flat Top` im realen Adeept-Batteriefach ohne Anpassung sauber kontaktiert
- wie Lade- und Sicherheitskonzept fuer die gewaehlten Zellen im Projektalltag dokumentiert werden
- ob fuer Bench- oder Dauerbetrieb zusaetzlich ein externer geregelter Strompfad vorgesehen wird

## Verwandte Projektdokumente

- `README.md` fuer den Gesamtprojektkontext
- `hardware/assembly/README.md` fuer die Pflicht zum Stock-Baseline-Test
- `hardware/bringup/README.md` fuer die Reihenfolge von Strom- und Erstinbetriebnahme
- `hardware/bom/README.md` fuer Beschaffung, Lieferumfang und Akkuauswahl
- `hardware/electronics/POWER_SUPPLY_CONCEPT.md` fuer Stock- und Projekt-Strompfad
- `firmware/arduino_arm/README.md` fuer die Original-Boardbasis des Servo-Teils
- `calibration/servo_limits/README.md` fuer spaetere Grenzwerte nach dem realen Aufbau
- `docs/templates/HARDWARE_INVENTORY_TEMPLATE.md` fuer die spaetere Bestandserfassung
- `../official_downloads/ADEEPT_V4_PACKAGE_REVIEW.md` fuer die zusammengezogene Auswertung des importierten Herstellerstands

## Recherchequellen

- [Adeept 5-DOF Robotic Arm Kit Compatible with Arduino IDE](https://www.adeept.com/adeept-5-dof-robotic-arm-kit-compatible-with-arduino-ide-programmable-diy-coding-stem-educational-5-axis-robot-arm-with-oled-display-processing-code_p0360.html) fuer Produktidentitaet, `ATMEGA328P`-/`Arduino UNO R3`-Hinweis, Package List, `0.96'' OLED Display`, `6x AD002 Servo` und den Hinweis auf `2x18650` oder `Micro USB`.
- [Adeept Learn: New Robotic Arm Kit for Arduino - V4.0](https://www.adeept.com/learn/detail-64.html) fuer die offizielle Tutorial- und Ressourcenbasis des Kits.
- [Adeept Video: Adeept 5-DOF Robotic Arm Assembly Tutorial | ADA031 Tutorial](https://www.adeept.com/video/detail-164.html) fuer den offiziellen Assembly-Bezug des Stock-Systems.
- [Adeept Forum: Adeept 5-DOF Arduino ROBOT ARM](https://www.adeept.com/forum/thread-1009-1-1.html) fuer die offizielle Support-Aussage, dass `Micro USB` bei Mehrservo-Betrieb zu wenig Strom liefern kann und `18650`-Batterien empfohlen werden.
- [Amazon: Adeept 5-DOF Roboterarm-Kit](https://www.amazon.de/dp/B085C2S1N7?ref=ppx_yo2ov_dt_b_fed_asin_title) als nutzerseitige Kauf- und Identitaetsreferenz des vorhandenen Projektkits; technische Basisentscheidungen werden jedoch primaer an offiziellen Adeept-Quellen festgemacht.
- [Molicel INR-18650-M35A 3,6 Volt 3500mAh Li-Ion ohne Loetfahne](https://akkuplus.de/molicel-INR-18650-M35A-36-Volt-3500mAh-Li-Ion-ohne-Loetfahne) fuer die konkret gewaehlte Akkubasis des Projektstands.
- [XTAR VC4SL QC3.0 Ladegeraet](https://www.akkuteile.de/ladegeraete/xtar/xtar-vc4sl-qc3-0-ladegeraet-fuer-li-ion-3-6v-3-7v-und-nimh-1-2v-akkus_500237_2914) fuer das konkret gewaehlte Ladegeraet des Projektstands.
- [Adeept V4 Package Review](../official_downloads/ADEEPT_V4_PACKAGE_REVIEW.md) fuer die zusammengezogene Auswertung von `Packing List`, `Read me first`, Stock-Test, Servo90 und Originalcode des importierten Herstellerdownloads.
