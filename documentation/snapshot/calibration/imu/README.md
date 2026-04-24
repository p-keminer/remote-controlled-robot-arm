# calibration/imu

## Zweck

Dieser Ordner ist fuer IMU-bezogene Referenzdaten und Kalibrierregeln reserviert.

## Aktueller Stand

Die aktuelle Segmentzuordnung am Controller lautet seit 2026-04-22: `S0 = Hand/Wrist`, `S1 = Unterarm`, `S2 = Oberarm`.
Die Referenzpose fuer den entspannt herunterhaengenden Arm ist dokumentiert und wird sowohl vom Dashboard als auch vom ROS-2-Twin genutzt.
Weitere Referenzposen und die reale Armfreigabe folgen noch.

## Inhalt

- Zuordnung von IMUs zu Hand/Wrist (S0), Unterarm (S1) und Oberarm (S2)
- Achsorientierung und eventuelle Invertierungen
- Nullpunkt- oder Referenzkalibrierung fuer Start und Re-Kalibrierung

## Regeln

- jede Sensorposition klar benennen
- Aenderungen an der S0/S1/S2-Zuordnung immer mit Datum dokumentieren
- Abweichungen in der Montage nicht implizit behandeln, sondern dokumentieren

## Schnittstellen/Abhaengigkeiten

- folgt `../README.md` und `../../CALIBRATION_FRAMEWORK.md`
- beeinflusst `../../firmware/esp32_controller/`, `../../dashboard/`, `../../ros2/` und `../../hardware/mechanik/`
