# Sensor Mounting Record

## Sensorzuordnung

- Datum: 2026-04-02
- Bearbeiter: p-keminer
- Aufbauzustand: Bench-Prototyp, Sensoren noch nicht am Koerper montiert

| Sensor | Koerper-/Aufbauposition | Mux-Kanal | I2C-Adresse | Befestigung | Besonderheiten |
| --- | --- | --- | --- | --- | --- |
| BNO055 #0 (S0) | Oberarm | PCA9548A Kanal 0 | 0x29 (ADR=3V3) | Bench-Breadboard | LED Rot GPIO6 am Controller |
| BNO055 #1 (S1) | Unterarm | PCA9548A Kanal 1 | 0x29 (ADR=3V3) | Bench-Breadboard | LED Gelb GPIO5 am Controller |
| BNO055 #2 (S2) | Hand/Wrist | PCA9548A Kanal 2 | 0x29 (ADR=3V3) | Bench-Breadboard | LED Gruen GPIO4 am Controller |
| Flex-Sensor | Handruecken (geplant) | — | GPIO1 ADC1 | Bench-Breadboard | 10kOhm Pull-Down, gerade=1108, gebogen=940 |
| PCA9548A Mux | — | — | 0x70 | Bench-Breadboard | 3 aktive Kanaele (0/1/2) |

## Beobachtungen

- Alle drei BNO055 sind GY-BNO055 Clones (Senzooe, Amazon B0D2J5PY29) — kein Adafruit-Breakout
- Kalibrierungsoffsets werden persistent im NVS gespeichert (Auto-Save bei Gyro>=3, Accel>=2, Mag>=2)
- Einzelkalibrierung per Serial moeglich: CAL0/CAL1/CAL2, RECAL, STOP
- Mux-Delay 10ms zwischen Kanalwechseln (nicht weiter optimieren, niedrige PPS akzeptiert)

## Relevanz fuer Kalibrierung

- IMU-Referenzsystem und Achszuordnung muessen nach Koerpermontage neu beschrieben werden
- Flex-Sensor muss in realer Montageposition erneut kalibriert werden (finales Messfenster offen)
- Referenzposen fuer Start und Wiederkalibrierung noch nicht festgeschrieben

## Verweis auf Fotos

- `docs/photos/2026-03-26_bench_aufbau_breadboards.jpg` — Bench-Breadboard-Uebersicht
- `docs/photos/2026-03-26_bench_aufbau_i2c_detail.jpg` — I2C-Verkabelungsdetail
- `docs/photos/2026-03-28_bench_aufbau_aktuell.jpg` — aktueller Bench-Stand mit allen drei ESPs
