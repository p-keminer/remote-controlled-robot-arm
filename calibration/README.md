# Kalibrierung und Realarm-Mapping

Diese Datei ist die kompakte Referenz fuer die aktive Sensorzuordnung,
Kalibrierung, Referenzpose, Greiferanker, Servogrenzen und die tatsaechliche
Realarm-Abbildung. Fuer die reale Servoausgabe ist
[`live_i2c_mapping.h`](../firmware/esp32_receiver/live_i2c_mapping.h) die
Implementierungsquelle; Dashboard und ROS sind separate Twin-Abbildungen.

Historische Flex-Sensor-, Breadboard- und 90-Grad-Montagestaende sind keine
aktiven Betriebsparameter.

## Sensorzuordnung

| Index | PCA9548A-Kanal | Segment | BNO055-Adresse |
| --- | ---: | --- | --- |
| `S0` | `0` | Hand/Wrist | `0x29` |
| `S1` | `1` | Unterarm | `0x29` |
| `S2` | `2` | Oberarm | `0x29` |

Der Multiplexer hat die Adresse `0x70`; Controller-SDA/SCL liegen auf
`GPIO8/GPIO9`. Montageposition und Achsausrichtung muessen bei jedem erneuten
Anlegen reproduziert werden. Eine verdrehte IMU macht gespeicherte Offsets oder
Referenzwerte nicht automatisch wieder passend.

## BNO055-Kalibrierung

Der Controller speichert fuer jeden Sensor eigene Offsets im NVS-Namespace
`imu_kalib` und laedt sie beim Boot. Automatisch gespeichert wird erst, wenn
`Gyro`, `Accel` und `Mag` jeweils mindestens Status `3` fuer drei aufeinander
folgende Pruefungen erreichen. `Sys` ist kein Speicherkriterium.

Serielle Befehle bei `115200 Baud`:

| Befehl | Wirkung |
| --- | --- |
| `CAL0`, `CAL1`, `CAL2` | nur den entsprechenden Sensor kalibrieren; bestehende Offsets bleiben bis zu einer erfolgreichen neuen Speicherung erhalten |
| `STOP` | laufende Einzelkalibrierung abbrechen |
| `CALSTAT` | NVS-, effektiven und aktuellen Live-Status aller Sensoren anzeigen |
| `RECAL` | alle drei gespeicherten Offsetsaetze loeschen |

Vorgehen:

1. Sensor fest in seiner realen Montageposition befestigen und Servoenergie
   getrennt lassen.
2. `CALn` fuer genau einen Sensor starten.
3. Fuer Gyro ruhig halten, fuer Accel mehrere feste Lagen durchlaufen und fuer
   Mag langsam um alle Achsen bewegen, fern von Servo-, Akku-, Metall- und
   USB-Stoerquellen.
4. Speicherungsmeldung abwarten und danach mit `CALSTAT` pruefen.
5. Referenzpose erneut aufnehmen, wenn Montage oder Achsausrichtung geaendert
   wurde.

Die Hintergrunddiagnose ist im normalen Livepfad bewusst aus, weil zusaetzliche
BNO055-Abfragen Paketpausen erzeugen koennen.

## Wearable-Referenzpose

Arbeitsreferenz vom 2026-04-23: Arm seitlich am Koerper entspannt
herunterhaengend, Schulter locker, Unterarm nicht aktiv gebeugt, Hand neutral.

| Sensor | Heading | Roll | Pitch |
| --- | ---: | ---: | ---: |
| `S0` Hand/Wrist | 255.0 Grad | -5.5 Grad | 65.8 Grad |
| `S1` Unterarm | 260.3 Grad | -10.7 Grad | 86.0 Grad |
| `S2` Oberarm | 280.8 Grad | 13.2 Grad | 88.9 Grad |

Diese Messung ist eine reproduzierbare Arbeitsreferenz, keine anatomische
Weltkoordinate. Der Realarm-Code verwendet sie nicht fuer jede Achse als
Mittelpunkt: insbesondere besitzt die aktive Schulterabbildung eine separate
Heading-Referenz von `240 Grad`.

## Greifer-Potentiometer

Aktiver Aufbau: aeusserer Poti-Pin an `3.3 V`, Wischer an `GPIO1`, vorhandener
10-kOhm-Pull-down von ADC nach GND. Die Firmware mittelt 16 ADC-Messungen,
filtert mit `alpha=0.18` und verwendet eine Rohwert-Totzone von 2 Counts.

| Zustand | ADC-Anker | Protokollwert `f` |
| --- | ---: | ---: |
| offen / Ruhelage | 1935 | 0 % |
| geschlossen | 3020 | 100 % |

Zwischenwerte werden linear abgebildet und auf `0..100 %` begrenzt. Das Feld
heisst aus Kompatibilitaetsgruenden weiterhin `flex_prozent`, obwohl der aktive
Sensor ein Potentiometer ist. Werte unter 200 gelten als unplausibel; das offene
Ende des 2-Draht-Potis darf bis zum ADC-Maximum 4095 reichen.

## Aktive Realarm-Abbildung

Der Receiver berechnet zunaechst Zielbytes `0..255`; der Arduino mappt diese auf
die empirischen Servogrenzen. Die folgende Tabelle beschreibt den aktiven Code,
nicht eine ideale Kinematik.

| Achse | Realarm-Eingang und Begrenzung | Byte-/Servoausgabe |
| --- | --- | --- |
| Basis | `clamp(88.9 - S2.pitch, 0, 100)` | linear `0..255` -> 12..139 Grad |
| Schulter | normierte Differenz `S2.heading - 240`, begrenzt auf `-60..60` | linear `0..255` -> 35..142 Grad |
| Ellbogen | Referenzrelation `S1.heading - S2.heading = -20.5`; `Referenz - aktuelle Relation`, begrenzt auf `0..45` | linear `0..255` -> 80..175 Grad |
| Handgelenk | negative relative Roll-Differenz `S0` zu `S1`, Offset `+10.2 Grad`, sicher begrenzt auf `-75..75 Grad` | Kodierfenster `-90..90` -> 5..177 Grad |
| Greifer | `f`, begrenzt auf `0..100 %` | linear `0..255` -> 32..126 Grad |

Besonderheiten des Handgelenks:

- Wenn der absolute Hand-Roll groesser als `35 Grad` ist und die relative
  Roll-Differenz kleiner als `25 Grad`, verwendet der Realarm einen
  Absolut-Roll-Fallback.
- Spruenge groesser als `90 Grad` werden gehalten.
- In der Fold-Zone ab `abs(S0.pitch) >= 145 Grad` greift eine Latch-/Hold-Logik.
- `WRISTZERO` setzt den Mittelpunkt beim naechsten gueltigen Paket neu;
  `WRISTSTAT` zeigt Rohwert, Ziel und Offset.

Bei Nullframes haelt die Schulter ihren letzten Wert; der Ellbogen haelt nach
der ersten Initialisierung ebenfalls den letzten Wert, wenn `S1` oder `S2`
komplett Null liefern. Zusaetzlich haelt bereits der Controller den letzten
gueltigen IMU-Wert bei einzelnen Nullframes.

## Achsenfreigabe und Glattung

Im committed Receiver-Default ist Live-I2C aus und der Test-Sweep an. Im
Live-Build wird jede nicht explizit freigegebene Achse auf Byte `128` gefuehrt.
Der Receiver begrenzt die Aenderung pro 20-ms-Frame auf:

- Basis und Schulter: 2 Byte
- Ellbogen und Handgelenk: 8 Byte
- Greifer: 12 Byte

Der Arduino begrenzt danach jede physische Achse erneut auf 3 Grad pro 20 ms.
Timeout, Software-Stop und Betriebsfreigabe sind in
[`SAFETY.md`](../SAFETY.md) beschrieben.

## Servogrenzen und Rueckfallwerte

| Gelenk | Pin | Min | Max | aktiver Mittel-/Rueckfallwert |
| --- | ---: | ---: | ---: | ---: |
| Basis | `D9` | 12 Grad | 139 Grad | 75 Grad |
| Schulter | `D11` | 35 Grad | 142 Grad | 88 Grad |
| Ellbogen | `D3` | 80 Grad | 175 Grad | 127 Grad |
| Handgelenk | `D5` | 5 Grad | 177 Grad | 91 Grad |
| Greifer | `D6` | 32 Grad | 126 Grad | 79 Grad |

Die Werte sind am Prototyp empirisch ermittelt. `90 Grad` bleibt nur als
historische Montage-/Erstmessposition relevant und ist nicht die aktive
Rueckfallpose.

## Abgrenzung zum Digital Twin

Dashboard und ROS visualisieren denselben Sensorstrom, sind aber aktuell nicht
bitgenau identisch mit dem Realarm-Mapping:

- Der Twin bezieht die Schulter auf `S2.heading = 280.8 Grad`, invertiert die
  Differenz und nutzt `0..60`; der Realarm bezieht sie auf `240 Grad` und nutzt
  `-60..60`.
- Die Basis verwendet denselben Pitch-Eingang, aber eine andere
  Ausgaberichtung/-skala.
- Der Realarm besitzt den beschriebenen Wrist-Absolut-Roll-Fallback; die
  Twin-Abbildung bildet diesen Sonderfall nicht identisch ab.

Deshalb ist der Twin ein Debug- und Beobachtungspfad, keine kalibrierte
Istpositionsanzeige des Servos. Aenderungen an Twin oder Realarm duerfen nicht
als automatisch synchron angenommen werden.

## Implementierungsquellen

- [Controller-Sensorik und NVS-Kalibrierung](../firmware/esp32_controller/esp32_controller.ino)
- [Realarm-Mapping](../firmware/esp32_receiver/live_i2c_mapping.h)
- [Receiver-Freigaben und Byte-Slew](../firmware/esp32_receiver/esp32_receiver.ino)
- [Arduino-Limits und Rueckfallwerte](../firmware/arduino_arm/arduino_arm.ino)
- [Dashboard-Mapping](../dashboard/web/js/robotarm_3d.js)
- [ROS-2-Mapping](../ros2/src/robotarm_description/robotarm_description/mapping.py)
