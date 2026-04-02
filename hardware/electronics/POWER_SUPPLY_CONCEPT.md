# Power Supply Concept

Dieses Dokument beschreibt die Stromversorgungsstrategie fuer den Adeept-Arm, den Stock-Test und den spaeteren Projektbetrieb.

## Ziel

- die offizielle Adeept-Strombasis sauber vom spaeteren Projekt-Strompfad trennen
- `Micro USB`, Batteriefach und spaetere Projektversorgung nicht vermischen
- Akku-/Batteriekauf auf dokumentierte Kriterien statt auf spontane Annahmen stuetzen
- verhindern, dass Servostrom spaeter die Logikpfade von Arduino oder ESP32 instabil macht

## Offizielle Adeept-Basis

Aus offizieller Adeept-Doku und dem importierten V4.0-Download ergibt sich folgende Herstellerbasis:

- das Kit besitzt ein `18650x2 Battery Holder`
- zwei `18650`-Zellen muessen laut Hersteller selbst beschafft werden
- `Read me first.pdf` fordert formale und qualifizierte `18650`-Lithiumzellen
- das Stock-Test-Tutorial fuehrt zuerst das Batteriefach an das Driver Board
- `Micro USB` wird fuer Upload, Port-Erkennung und serielle Kommunikation verwendet
- die Produktseite nennt `Micro USB` zwar als moegliche Alternative, die offizielle Support-Aussage warnt aber bei Mehrservo-Bewegung vor zu wenig Strom

## Verbindlicher Projektarbeitsstand

- `Micro USB` wird **nicht** als verlaesslicher Vollbetriebs-Strompfad fuer den aufgebauten Mehrservo-Arm behandelt
- fuer Stock-Test und spaetere reale Bewegungsversuche wird ein dokumentierter externer Energiepfad vorausgesetzt
- vor den ersten echten Bewegungsversuchen muss entschieden sein, ob der Stock-kompatible `2x18650`-Pfad oder ein externer geregelter Bench-Pfad genutzt wird
- Servo-Strom und empfindliche Logikpfade muessen konzeptionell getrennt gedacht werden, auch wenn Signale spaeter gemeinsame Bezugsmasse benoetigen

## Aktuell ausgewaehlte Beschaffungsbasis

Fuer den derzeit dokumentierten Projektstand ist folgende Beschaffung vorgesehen:

- `4x Molicel INR-18650-M35A` ohne Loetfahne
- `1x XTAR VC4SL`

Dazu gelten fuer den Start folgende Annahmen:

- `Flat Top` wird vorerst bevorzugt; ein erhoehter Pluspol wird erst bei realem Batteriefachbedarf geprueft
- das `XTAR VC4SL` darf alltagstauglich auch mit normaler USB-C-Stromquelle betrieben werden; `QC3.0` oder `PD` werden nur fuer maximale Ladeleistung benoetigt
- ein Laptop-Netzteil oder anderes USB-C-Netzteil ist damit als langsamerer Alltags-Ladepfad zulaessig, sofern Ausgang, Kabel und Stecker sauber passen

## Drei Versorgungsstufen

### Stufe 1 - Stock-Baseline des Originalkits

Ziel:

- offiziellen Adeept-Aufbau und Originalfunktion nach Herstellerpfad pruefen

Erlaubter Pfad:

- offizielles Batteriefach mit `2x18650` gemaess Herstellerbasis
- `Micro USB` fuer Upload und Port-Erkennung

Nicht als Schlussfolgerung fuer spaeter behandeln:

- ein erfolgreicher Upload ueber `Micro USB` beweist nicht, dass `Micro USB` fuer realen Mehrservo-Betrieb ausreicht

### Stufe 2 - Projekt-Bench

Ziel:

- Arduino, Receiver-ESP, Sensorpfad und I2C/ESP-NOW pruefen

Bevorzugter Pfad:

- Logikpfade getrennt und kontrolliert versorgen
- Servopfad ueber einen eigenen, dokumentierten Strompfad fuehren
- gemeinsame Bezugsmasse zwischen Signal- und Leistungspfad nur bewusst herstellen, nicht zufaellig ueber USB oder Steckbrettverkabelung

### Stufe 3 - Dauerbetriebsnaher Projektstand

Ziel:

- stabiler Betrieb mit dokumentierter Trennung von Lastpfad und Logikversorgung

Pflichtpunkte:

- definierter Servostrompfad
- definierter Logikstrompfad fuer Arduino und ESP32
- nachvollziehbare Absicherung, Steckerbelegung und Zugentlastung
- dokumentierter Lade- oder Wechselprozess fuer Akkus, falls ein Batteriepfad bleibt

## Kauf- und Auswahlkriterien fuer den naechsten Schritt

Diese Punkte muessen vor der echten Beschaffung dokumentiert und abgehakt werden:

- passt die Energiequelle zum Stock-Batteriefach oder wird bewusst ein anderer Projektpfad gewaehlt
- ist der Strompfad fuer mehrere gleichzeitig anfahrende Servos ausgelegt
- ist die Versorgung fuer Upload, Bench und Bewegungsbetrieb sauber getrennt beschrieben
- sind Lade- und Sicherheitsregeln fuer die gewaehlte Akkuvariante dokumentiert
- ist fuer den Bench-Betrieb ein reproduzierbarer externer Versorgungsweg vorhanden

## Erste Best-Practice-Entscheidungen

- Servolast wird nicht stillschweigend ueber einen USB-Uploadpfad freigegeben
- die Projektdokumentation behandelt hohe Servolast und Logikversorgung als zwei unterschiedliche Planungsprobleme
- bei spaeteren Erweiterungen mit ESP32, LEDs und Sensorik soll die Servoenergie nicht unkontrolliert ueber empfindliche Controllerpfade mitlaufen
- ein gemeinsamer Ground-Bezug bleibt fuer Signalintegritaet mitzudenken, ersetzt aber keine getrennte Leistungsplanung

## Offene Punkte

- reale Stromaufnahme des aufgebauten Adeept-Arms messen
- entscheiden, ob fuer den ersten echten Bewegungsbetrieb `2x18650` oder ein geregelter externer Bench-Pfad bevorzugt wird
- reale Batteriefach-Passung der ausgewaehlten `Flat Top`-Zellen pruefen
- dokumentieren, ob fuer den Ladealltag ein bestimmtes USB-C-Netzteil fest als Projektzubehoer hinterlegt werden soll
- dokumentieren, wie Arduino-, Receiver-ESP- und Sender-ESP-Versorgung im spaeteren Gesamtaufbau zueinander stehen

## Verwandte Projektdokumente

- `ESP_INTEGRATION_CONCEPT.md`
- `../ADEEPT_ARM_PRODUCT_BASELINE.md`
- `../bringup/README.md`
- `../bom/README.md`
- `../../preparation/robot_arm_readiness/README.md`
- `../../SAFETY_FRAMEWORK.md`

## Recherchequellen

- [Adeept 5-DOF Robotic Arm Kit Compatible with Arduino IDE](https://www.adeept.com/adeept-5-dof-robotic-arm-kit-compatible-with-arduino-ide-programmable-diy-coding-stem-educational-5-axis-robot-arm-with-oled-display-processing-code_p0360.html) fuer `2x18650`, `Micro USB` und die offizielle Produktbasis.
- [Adeept Forum: Adeept 5-DOF Arduino ROBOT ARM](https://www.adeept.com/forum/thread-1009-1-1.html) fuer die offizielle Support-Aussage, dass `Micro USB` bei Mehrservo-Betrieb zu wenig Strom liefern kann.
- [Read me first.pdf](../../official_downloads/extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Read%20me%20first.pdf) fuer die Pflicht, qualifizierte `18650`-Zellen zu beschaffen.
- [_2 Packing List.pdf](../../official_downloads/extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Tutorials-pdf/_2%20Packing%20List.pdf) fuer `18650 Battery x2` als Selbstbeschaffung und das `18650x2 Battery Holder`.
- [_5 Test Adeept Arm Drive Board, Servo and etc..pdf](../../official_downloads/extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Tutorials-pdf/_5%20Test%20Adeept%20Arm%20Drive%20Board,%20Servo%20and%20etc..pdf) fuer den offiziellen Stock-Testfluss mit Batteriefach und Driver Board.
- [How to let the Robotic Arm learn our control action.pdf](../../official_downloads/extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Tutorials-pdf/8.%20How%20to%20let%20the%20Robotic%20Arm%20learn%20our%20control%20action.pdf) fuer den Hinweis auf `Powered by a fully charged battery`.
- [Adafruit Trainable Robotic Arm - Wiring](https://learn.adafruit.com/trainable-robotic-arm/wiring) fuer die Best-Practice-Empfehlung, mehrere Servos nicht implizit nur ueber den Arduino zu versorgen und fuer fuenf Servos eine staerkere, getrennte Stromquelle mitzudenken.
- [Arduino Help Center - What power supply can I use with my Arduino board?](https://support.arduino.cc/hc/en-us/articles/360018922259-What-power-supply-can-I-use-with-my-Arduino-board) fuer die allgemeine Arduino-Grundregel, dass bei zusaetzlichem Leistungsbedarf ein passender externer Strompfad vorgesehen werden muss.
- [Molicel INR-18650-M35A 3,6 Volt 3500mAh Li-Ion ohne Loetfahne](https://akkuplus.de/molicel-INR-18650-M35A-36-Volt-3500mAh-Li-Ion-ohne-Loetfahne) fuer die konkret gewaehlte Akkubasis des Projektstands.
- [XTAR VC4SL QC3.0 Ladegeraet](https://www.akkuteile.de/ladegeraete/xtar/xtar-vc4sl-qc3-0-ladegeraet-fuer-li-ion-3-6v-3-7v-und-nimh-1-2v-akkus_500237_2914) fuer das konkret gewaehlte Ladegeraet.
- [XTAR VC4SL Product Page](https://www.xtar.cc/product/XTAR-VC4SL-Charger-178.html) fuer die Herstellerangabe, dass `QC3.0` oder `PD` nur fuer maximale Ladegeschwindigkeit noetig sind.
