# Pin Mapping Research

Dieses Dokument sammelt den ersten GPIO-Plan fuer `ESP32-S3 DevKitC-1 N16R8`.
Die Zuordnung ist bewusst als **bevorzugter Startplan** dokumentiert und noch nicht als endgueltig eingefroren.

## Ziel

- problematische Pins frueh vermeiden
- Sender und Receiver mit moeglichst einfachen, gut debugbaren Pinbildern planen
- Flex-Sensor, IMUs, UART, LEDs und Buzzer ohne unnoetige Zielkonflikte anordnen

## Grundannahme

Die vorhandenen Boards sind `ESP32-S3-WROOM-1-N16R8` auf DevKitC-Traeger, **Boardrevision v1.0** (bestätigt 2026-03-22).
4 Boards verfuegbar: 2 aktiv als Sender und Receiver, 2 als Reserve.
Onboard Komfortfunktionen werden nicht als kritischer Primaerpfad eingeplant — die vorhandene RGB-LED auf `GPIO48` ist fuer die aktuellen Boards bestaetigt, bleibt aber wegen spaeterer Hardwareaustauschbarkeit nur Zusatzanzeige.

## Pins mit besonderer Vorsicht

| Pin / Bereich | Umgang im Projekt | Grund |
| --- | --- | --- |
| `GPIO0`, `GPIO3`, `GPIO45`, `GPIO46` | fuer Alltags-I/O vermeiden | Strapping-Pins |
| `GPIO19`, `GPIO20` | in fruehen Revisionen freihalten | native USB- und USB-nahe Debugfunktionen nicht frueh verbauen |
| `GPIO43`, `GPIO44` | fuer serielle Diagnose moeglichst freihalten | Standard-UART0 fuer Logs und Bringup praktisch |
| `GPIO35`, `GPIO36`, `GPIO37` | **nicht nutzbar** — intern belegt | Octal-PSRAM (OPI) auf `N16R8` belegt diese Pins fest; bestätigt fuer die vorhandenen Boards |
| `GPIO48` | nur als Zusatzfunktion nutzen | onboard RGB-LED auf **Revision v1.0** liegt auf `GPIO48`; wird nicht als einziger Statuspfad eingeplant |
| `GPIO26` bis `GPIO37` | in v1 konservativ meiden | reduziert fruehe Konflikte und haelt Abstand zu problematischen Bereichen |

## Bevorzugter Sender-Pinplan

| Signal | Bevorzugter Pin | Grundgedanke |
| --- | --- | --- |
| `I2C SDA` fuer IMUs / Mux | `GPIO8` | gut erreichbar, nicht in den bewusst gemiedenen Bereichen |
| `I2C SCL` fuer IMUs / Mux | `GPIO9` | direkt benachbart zu `GPIO8`, erleichtert saubere Fuehrung |
| `Flex-Sensor ADC` | `GPIO1` | `ADC1`, damit kein frueher Konflikt mit Funkbetrieb ueber `ADC2` entsteht |
| `LED Oberarm-IMU` | `GPIO4` | einfache, gut sichtbare Debugleitung |
| `LED Unterarm-IMU` | `GPIO5` | einfache, gut sichtbare Debugleitung |
| `LED Hand/Wrist-IMU` | `GPIO6` | einfache, gut sichtbare Debugleitung |
| `LED COMMS optional` | `GPIO7` | zusaetzlicher Sender-Status moeglich |
| `LED FAULT optional` | `GPIO10` | zusaetzlicher Fehlerpfad moeglich |
| `Buzzer optional` | `GPIO21` | getrennt von den IMU-LEDs und nicht an revisionsabhaengige Komfortfunktionen gebunden |

## Bevorzugter Receiver-Pinplan

| Signal | Bevorzugter Pin | Grundgedanke |
| --- | --- | --- |
| `UART TX -> Arduino RX` | `GPIO15` | trennt die Robotik-UART bewusst von USB-/Debugpfaden |
| `UART RX <- Arduino TX` | `GPIO16` | Paarbildung mit `GPIO15` fuer einfache Verdrahtung |
| `LED LINK` | `GPIO4` | gut sichtbarer Grundstatus |
| `LED UART` | `GPIO5` | sichtbarer Weitergabestatus |
| `LED FAULT` | `GPIO6` | klarer Fehlerpfad |
| `Buzzer` | `GPIO21` | eigener Warnkanal, getrennt von UART und Status-LEDs |

## Umgang mit der onboard RGB-LED

- Die vorhandenen Boards sind **Revision v1.0** — die onboard RGB-LED liegt auf **`GPIO48`** (bestätigt 2026-03-22).
- Revision v1.1 haette die RGB-LED auf `GPIO38` — diese Revision ist im Projekt nicht vorhanden.
- Die onboard RGB-LED wird trotzdem nur als **zusaetzliche globale Komfortanzeige** behandelt, da sie revisionsabhaengig bleibt und kein stabiler Pfad fuer den Dauerbetrieb sein soll.
- Wenn ein stabiler RGB-Pfad gebraucht wird, bleibt eine externe RGB-LED oder drei feste Einzel-LEDs die vorzuziehende Loesung.

## Warum `ADC1` fuer den Flex-Sensor?

Der Flex-Sensor soll bevorzugt auf `ADC1` bleiben, weil `ADC2` auf ESP32-S3 im Zusammenspiel mit aktiviertem Funkbetrieb problematisch sein kann.
Da `ESP-NOW` den Wi-Fi-Block nutzt, ist diese Trennung fuer das Projekt frueh sinnvoll.

## Warum `GPIO8` und `GPIO9` fuer I2C?

I2C ist auf ESP32-S3 nicht auf einen einzigen harten Pinzwang reduziert.
Die Wahl `GPIO8` / `GPIO9` ist hier eine Projektentscheidung, weil die Pins:

- nah beieinander liegen
- fuer ein kompaktes I2C-Paar gut zu fuehren sind
- nicht in den zuerst gemiedenen Reservierungsbereichen liegen

## Warum `GPIO15` und `GPIO16` fuer die Receiver-UART?

Der Receiver soll seine Robotik-UART moeglichst nicht auf denselben Pfaden fuehren, die fuer fruehes Flashing, USB-JTAG oder serielle Diagnose bequem sind.
`GPIO15` und `GPIO16` sind deshalb die bevorzugte erste Wahl.

Fallback:

- Wenn das reale Carrier- oder Leitungsbild unguenstig ist, kann als Reserve auf ein anderes, bench-validiertes UART-Paar gewechselt werden.
- `GPIO17` / `GPIO18` bleiben die naechste sinnvolle Reserve, weil sie auf dem DevKit bereits als `U1TXD` / `U1RXD` herausgefuehrt sind.

## Hinweis zu Bootzustand und sicheren Ausgaengen

LEDs koennen kurze Startzuckungen tolerieren.
Der Buzzer darf das nicht.
Deshalb wird fuer den Buzzer zusaetzlich ein Treiber- oder Schaltpfad mit sicherem Default-Aus-Zustand empfohlen, statt sich nur auf den nackten GPIO-Pegel waehrend des Bootens zu verlassen.

## Validierungsreihenfolge

1. reale Boardrevision identifizieren — **bestaetigt v1.0, RGB auf `GPIO48` (2026-03-22)**
2. Einzelsensor auf `GPIO8` / `GPIO9` pruefen — **bestaetigt: BNO055 auf 0x29 (ADR=3V3), Rohwerte stabil, Gyro 3/3 (2026-03-22)**
3. Mux PCA9548A pruefen — **bestaetigt: Adresse 0x70, BNO055 ueber Kanal 0, Sys/Gyro/Accel 3/3 (2026-03-22)**
4. Zwei BNO055 gleichzeitig ueber den Mux pruefen — **bestaetigt: Kanaele 0 und 1 stabil, beide Sensoren valide (2026-03-22)**
5. Flex-Sensor auf `GPIO1` pruefen — **bestaetigt: gerade=1108, gebogen=940, Bereich 168 Counts (2026-03-22)**
6. Sender-LEDs pruefen
7. Receiver-UART auf `GPIO15` / `GPIO16` pruefen
8. Buzzerpfad mit sicherem Default-Off pruefen
9. dritten IMU auf dem Mux-Pfad und weitere Mehr-IMU-Erweiterung pruefen

## Recherchequellen

- [ESP32-S3-DevKitC-1 User Guide v1.1](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.1.html) fuer das aktuelle DevKit-Pinlayout und die onboard RGB-LED auf `GPIO38`.
- [ESP32-S3-DevKitC-1 User Guide v1.0](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.0.html) fuer die aeltere DevKit-Variante mit onboard RGB-LED auf `GPIO48`.
- [ESP32-S3 GPIO & RTC GPIO](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/peripherals/gpio.html) fuer GPIO-Reservebereiche, Strapping-Hinweise und GPIO-Matrix-Grundlagen.
- [ESP32-S3 ADC Documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/peripherals/adc/index.html) fuer ADC1/ADC2-Grundlagen und die ADC2-Einschraenkungen bei aktivem Wi-Fi-Betrieb.
- [ESP32-S3 Series Datasheet](https://documentation.espressif.com/api/resource/doc/file/rz94aWY3/FILE/esp32-s3_datasheet_en.pdf) fuer die Belegung hoeherer GPIOs bei Flash-/PSRAM-Konfigurationen.
