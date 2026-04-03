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

## Kabelfuehrung — Hinweise fuer die Wearable-Montage

### I2C-Kabel (Controller → PCA9548A → BNO055)

- Bei 100 kHz (Standard-Mode) sind Kabellaengen bis ca. 1 Meter unproblematisch
- Bus-Kapazitaet bei aktuellem Setup (~75 pF) weit unter dem I2C-Limit von 400 pF
- Der PCA9548A hilft: immer nur ein Mux-Kanal aktiv, Kapazitaet der anderen Sensoren abgekoppelt
- **Verdrillte Paare** empfohlen (SDA+GND, SCL+GND) statt lose parallel laufende Draehte
- Kabel nicht direkt neben Servo-/Motorleitungen verlegen (PWM-Stoerungen)
- Pull-Ups sind auf PCA9548A und BNO055-Boards bereits vorhanden — bei unter 50 cm kein Zusatz noetig
- 25 cm (Controller am Koerper → Oberarm-IMU) ist unkritisch

### Flex-Sensor-Kabel (analoges Signal)

- Analogsignale sind empfindlicher gegen Stoerungen als I2C
- Das ADC-Messfenster ist klein (nur 168 Counts: 1108 gerade → 940 gebogen) — Rauschen faellt stark ins Gewicht
- Bei 25+ cm Kabellaenge: **abgeschirmtes Kabel** empfohlen (Schirmung an GND) oder Signal+GND eng zusammen fuehren
- **Nicht neben Servo-/Motorleitungen** verlegen — PWM-Impulse koppeln ein
- Nach Montage: Flex-Sensor in Endposition halten und per Serial Monitor pruefen ob Werte stabil sind
- Falls Rauschen >20 Counts: Software-Gleitender-Mittelwert (5-10 Samples) oder 100nF Kondensator am ADC-Pin gegen GND

## Relevanz fuer Kalibrierung

- IMU-Referenzsystem und Achszuordnung muessen nach Koerpermontage neu beschrieben werden
- Flex-Sensor muss in realer Montageposition erneut kalibriert werden (finales Messfenster offen)
- Referenzposen fuer Start und Wiederkalibrierung noch nicht festgeschrieben

## Verweis auf Fotos

- `docs/photos/2026-03-26_bench_aufbau_breadboards.jpg` — Bench-Breadboard-Uebersicht
- `docs/photos/2026-03-26_bench_aufbau_i2c_detail.jpg` — I2C-Verkabelungsdetail
- `docs/photos/2026-03-28_bench_aufbau_aktuell.jpg` — aktueller Bench-Stand mit allen drei ESPs
