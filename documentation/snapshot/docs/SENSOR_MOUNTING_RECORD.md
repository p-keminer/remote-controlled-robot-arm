# Sensor Mounting Record

## Sensorzuordnung

- Datum: 2026-04-24
- Bearbeiter: p-keminer
- Aufbauzustand: aktueller Wearable- und Twin-Arbeitsstand

| Sensor / Eingabe | Koerper- oder Aufbauposition | Mux-Kanal | I2C-Adresse / Pin | Befestigung | Besonderheiten |
| --- | --- | --- | --- | --- | --- |
| BNO055 #0 (`S0`) | Hand/Wrist | PCA9548A Kanal 0 | `0x29` | Wearable-Aufbau | entspricht Hand-/Wrist-Segment |
| BNO055 #1 (`S1`) | Unterarm | PCA9548A Kanal 1 | `0x29` | Wearable-Aufbau | entspricht Unterarm-Segment |
| BNO055 #2 (`S2`) | Oberarm | PCA9548A Kanal 2 | `0x29` | Wearable-Aufbau | entspricht Oberarm-Segment |
| PCA9548A | Controller-nahe Elektronik | — | `0x70` | aktueller Prototyp | 3 aktive Kanaele `0/1/2` |
| Greifer-Potentiometer | Controller `GPIO1` | — | `ADC1` | aktueller 2-Draht-Aufbau | robuster Greifer-Pfad fuer `f` |

## Beobachtungen

- Aktuelle Segmentzuordnung: `S0 = Hand/Wrist`, `S1 = Unterarm`, `S2 = Oberarm`
- Die Referenzpose `calibration/reference_poses/2026-04-23_neutralpose_arm_haengend.md` dient aktuell als Twin-Arbeitsreferenz
- Dashboard und ROS 2 nutzen denselben Segment- und Referenzstand
- Der historische Flex-Sensor-Pfad ist nicht mehr die aktive Greifer-Eingabe und bleibt nur als Altstand dokumentiert

## Relevanz fuer Kalibrierung

- weitere Referenzposen fuer realen Bewegungsbetrieb stehen noch aus
- der aktuelle Greifer-Pfad wird unter `calibration/gripper_input/` dokumentiert
- reale Safety-Freigaben folgen erst nach dokumentiertem Security-/Safety-Uplift

## Verweis auf Fotos

- `docs/photos/2026-03-28_bench_aufbau_aktuell.jpg`
- `docs/photos/readme/20260423_213652000-ios.gif`
