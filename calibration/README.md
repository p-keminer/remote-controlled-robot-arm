# calibration

## Zweck

Dieser Ordner enthaelt alle Kalibrierregeln, Referenzdaten und dokumentierten Grenzwerte des Projekts.

## Aktueller Stand

Die Kalibrierstruktur trennt jetzt klar zwischen aktivem und historischem Greifer-Pfad:

- `imu/` und `reference_poses/` beschreiben den aktuellen Segment- und Referenzstand fuer Dashboard und ROS 2
- `gripper_input/` dokumentiert den aktuellen Potentiometer-Arbeitsstand
- `flex_sensor/` bleibt als historischer Prototyp- und Bench-Pfad erhalten
- `servo_limits/` haelt die real gemessenen Gelenkgrenzen fest

## Inhalt

- `imu/` fuer Sensorzuordnung, Achsen und Nullpunktbezug
- `gripper_input/` fuer den aktuellen Potentiometer-Arbeitsstand des Greifers
- `flex_sensor/` als historische Flex-Sensor-Dokumentation
- `servo_limits/` fuer mechanische Grenzwerte und sichere Rueckfallbereiche
- `reference_poses/` fuer Referenzhaltungen und Startposen

## Regeln

- Kalibrierwissen darf nicht nur im Code verborgen sein.
- Referenzdaten und daraus abgeleitete Grenzen bleiben getrennt dokumentiert.
- Historische Flex-Sensor-Staende bleiben sichtbar, gelten aber nicht als aktueller Kalibrierpfad.
- Aenderungen mit Auswirkung auf Sicherheit immer mit `SAFETY_FRAMEWORK.md` abstimmen.

## Schnittstellen / Abhaengigkeiten

- richtet sich nach `../CALIBRATION_FRAMEWORK.md`
- liefert Grenz- und Referenzwissen an `../firmware/`, `../dashboard/` und `../ros2/`
- wird durch Messungen in `../docs/measurement_logs/` ergaenzt
