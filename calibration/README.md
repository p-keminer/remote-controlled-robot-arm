# calibration

## Zweck

Dieser Ordner enthaelt alle Kalibrierregeln, Referenzdaten und dokumentierten Grenzwerte des Projekts.

## Aktueller Stand

Die Kalibrierstruktur ist angelegt, konkrete Mess- und Referenzdaten folgen in spaeteren Arbeitsschritten.

## Inhalt

- `imu/` fuer Sensorzuordnung, Achsen und Nullpunktbezug
- `flex_sensor/` fuer ADC-Bereiche und Greifermapping
- `servo_limits/` fuer mechanische Grenzwerte und sichere Rueckfallbereiche
- `reference_poses/` fuer Referenzhaltungen und Startposen

## Regeln

- Kalibrierwissen darf nicht nur im Code verborgen sein
- Referenzdaten und daraus abgeleitete Grenzen getrennt dokumentieren
- Aenderungen mit Auswirkung auf Sicherheit immer mit `SAFETY_FRAMEWORK.md` abstimmen

## Schnittstellen/Abhaengigkeiten

- richtet sich nach `../CALIBRATION_FRAMEWORK.md`
- liefert Grenz- und Referenzwissen an `../firmware/`
- wird durch Messungen in `../docs/measurement_logs/` ergaenzt
