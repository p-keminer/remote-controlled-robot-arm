# Flex-Sensor Rohmesswerte

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

## Offene Punkte

- **Neukalibrierung erforderlich:** Ruhewert (0%) wurde frei in der Luft gemessen, nicht in Montageposition. Kalibrierung muss mit montiertem Sensor wiederholt werden.
- **Kriech-Charakteristik (Creep) beobachtet:** Sensor kehrt nach starkem Biegen nur langsam in den Ruhewert zurueck. Das Polymermaterial folgt der mechanischen Rueckstellung mit Verzoegerung. Muss bei Filterung und Totzone beruecksichtigt werden.
- Montage am Handschuh noch nicht getestet — Werte koennen sich durch Montage aendern
- Sensoralterung und Temperatureinfluss noch nicht bewertet
- Totzone und Glaettung noch nicht definiert
