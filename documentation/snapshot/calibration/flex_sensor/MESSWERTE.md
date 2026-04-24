# Flex-Sensor Rohmesswerte

> Historischer Prototyp-Stand: Diese Messreihe dokumentiert den fruehen Flex-Sensor-Pfad.
> Der aktuelle Greifer-Arbeitsstand nutzt ein Potentiometer aus Robustheitsgruenden und wird unter `../gripper_input/` weitergefuehrt.

## Messaufbau (Stand 2026-03-22)

- Board: ESP32-S3-WROOM-1-N16R8 (Controller)
- Pin: GPIO1 (ADC1)
- Schaltung: Spannungsteiler, 10kΩ Pull-Down gegen GND
- ADC-Aufloesung: 12 Bit (0-4095)
- Referenzspannung: 3.3V
- Mittelwert ueber 16 Messungen pro Ausgabe
- Sketch: `firmware/flex_sensor_test/`

## Rohmessungen

| Position | Rohwert (ADC) | Spannung | Beschreibung |
| --- | --- | --- | --- |
| Gerade (0%) | 1108 | 0.893V | Sensor waagerecht, nicht gebogen |
| Maximal gebogen (100%) | 940 | 0.757V | Enden fast beruehrend (~0.5cm Abstand) |

Wertebereich: 168 Counts

## Mapping-Formel (abgeleitet)

```
biegung_prozent = (1108 - rohwert) / 168.0 * 100
```

- Rohwert 1108 → 0% (gerade)
- Rohwert 940 → 100% (maximal gebogen)
- Werte ausserhalb des Bereichs werden geclampt (0-100%)

## Wearable-Montage 2026-04-23 (aus Controller-Prozentwerten rueckgerechnet)

Der Controller-Sketch verwendete zu diesem Zeitpunkt noch die alten Bench-Anker `1108` und `940`.
Die folgenden Rohwerte wurden deshalb aus den angezeigten Prozentwerten rueckgerechnet:

```
rohwert ~= 1108 - 1.68 * prozent
```

| Position | beobachteter Prozentbereich | Mittelwert Prozent | abgeleiteter Rohwert | Hinweis |
| --- | ---: | ---: | ---: | --- |
| Finger ausgestreckt | 45.8% - 51.2% | 48.6% | ca. 1026 ADC | stabiler Block |
| Finger entspannt | 27.4% - 31.0% | 28.9% | ca. 1059 ADC | Ruhebereich |
| Finger komplett zusammen | 30.4% - 33.9% | 32.2% | ca. 1054 ADC | stabiler Block |

## Provisorische Wearable-Anker fuer spaeteres Mapping

Diese Werte sind bewusst als Arbeitsstand zu verstehen und noch **kein finales Greifermapping**.

| Rolle | Rohwert (ADC) | Herleitung |
| --- | ---: | --- |
| Offen / ausgestreckt | 1026 | Mittelwert aus stabilem Open-Block |
| Geschlossen / zusammen | 1056 | bewusst leicht ueber Mittelwert 1054 gesetzt, um den oberen Bereich etwas sensibler zu machen |
| Entspannter Ruhewert | 1059 | Mittelwert aus ruhiger, herunterhaengender Referenzpose |

## Controller-Arbeitsstand 2026-04-23 (historisch)

Der Controller-Sketch verwendet ab dem Wearable-Abgleich einen bewusst etwas
breiteren Arbeitsbereich, damit der Flex-Sensor im laufenden Betrieb nicht
sofort wieder auf `100%` saettigt:

| Rolle | Rohwert (ADC) | Verwendet im Sketch |
| --- | ---: | --- |
| Offen / ausgestreckt | 1026 | `FLEX_GERADE` |
| Geschlossen / mit Reserve | 1066 | `FLEX_GEBOGEN` |

Die obere Grenze liegt damit absichtlich ueber dem zuvor notierten
Provisorium `1056`, weil der Live-Betrieb noch zu fruehe Volllast (`100%`)
gezeigt hat.

## Einordnung Wearable-Stand

- `entspannt` und `komplett zusammen` liegen aktuell sehr nah beieinander.
- Der Sensor unterscheidet im aktuellen Einbau deutlich besser zwischen `ausgestreckt` und `nicht ausgestreckt` als zwischen `entspannt` und `komplett zusammen`.
- Fuer ein spaeteres proportionales Greifermapping ist die aktuelle Montage damit nur eingeschraenkt ideal.
- Fuer erste Schwellwertlogik oder ein provisorisches Greifermapping sind die oben genannten Wearable-Anker jedoch ausreichend.

## Offene Punkte des historischen Pfads

- Wearable-Montage 2026-04-23 ist dokumentiert; fuer den Endstand sind direkte Rohmessungen mit `firmware/flex_sensor_test/` trotzdem vorzuziehen.

- **Neukalibrierung erforderlich:** Ruhewert (0%) wurde frei in der Luft gemessen, nicht in Montageposition. Kalibrierung muss mit montiertem Sensor wiederholt werden.
- **Kriech-Charakteristik (Creep) beobachtet:** Sensor kehrt nach starkem Biegen nur langsam in den Ruhewert zurueck. Das Polymermaterial folgt der mechanischen Rueckstellung mit Verzoegerung. Muss bei Filterung und Totzone beruecksichtigt werden.
- Montage am Handschuh/Wearable ist jetzt grundsaetzlich getestet, das finale Mapping bleibt aber offen
- Sensoralterung und Temperatureinfluss noch nicht bewertet
- Totzone und Glaettung noch nicht definiert
