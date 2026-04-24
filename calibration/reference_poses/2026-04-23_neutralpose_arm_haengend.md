# Referenzpose 2026-04-23 - Arm entspannt herunterhaengend

## Kontext

- Datum: 2026-04-23
- Aufbau: Controller vollstaendig am Arm montiert
- Segmentzuordnung: `S0 = Hand/Wrist`, `S1 = Unterarm`, `S2 = Oberarm`
- Zweck: erste wiederholbare Wearable-Referenzpose fuer spaeteres IMU-zu-Servo-Mapping

## Posenbeschreibung

- Arm seitlich am Koerper, entspannt herunterhaengend
- Schulter entspannt, nicht bewusst angehoben
- Unterarm locker, nicht aktiv gebeugt
- Hand neutral und locker
- Greifer-Eingabe entspannt; der alte Flex-Wert aus diesem Datum bleibt nur als historischer Bezug notiert

## Referenzwerte

Abgeleitet aus den stabilen Statusbloecken `#5913`, `#5940` und `#5973`.
Die Werte sollen als Arbeitsreferenz dienen, nicht als anatomisch exakte Weltkoordinaten.

| Segment | Rolle | Heading | Roll | Pitch | Hinweis |
| --- | --- | ---: | ---: | ---: | --- |
| `S0` | Hand/Wrist | 255.0 | -5.5 | 65.8 | Hand locker |
| `S1` | Unterarm | 260.3 | -10.7 | 86.0 | Unterarm entspannt |
| `S2` | Oberarm | 280.8 | 13.2 | 88.9 | Oberarm entspannt |
| `Greifer-Eingabe` | entspannt | ca. 29% | - | - | historischer Altwert aus dem damaligen Flex-Mapping |

## Einordnung

- Fuer das spaetere Mapping sind relative Aenderungen zu dieser Pose wichtiger als die absoluten Heading-Werte.
- `Roll` und `Pitch` sind fuer die erste Mapping-Arbeit relevanter als `Heading`, solange das Magnetometer nicht als globale Fuehrung benutzt wird.
- Der historische Greifer-Wert ist hier nur als Ruhereferenz notiert; der aktuelle Potentiometer-Arbeitsstand liegt unter `calibration/gripper_input/`.

## Offene Punkte

- weitere Referenzpose mit deutlich gebeugtem Arm aufnehmen
- weitere Referenzpose mit angehobenem Arm aufnehmen
- Achsinvertierungen und Nullpunktlogik spaeter in `calibration/imu/` ausformulieren
