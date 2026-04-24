# calibration/reference_poses

## Zweck

Dieser Ordner enthaelt definierte Koerper- und Roboterposen fuer Start, Re-Kalibrierung und Vergleichsmessungen.

## Aktueller Stand

Die Struktur enthaelt eine erste aktive Wearable-Referenzpose fuer den entspannt herunterhaengenden Arm unter `2026-04-23_neutralpose_arm_haengend.md`.
Diese Pose dient aktuell als Arbeitsreferenz fuer Dashboard und ROS 2.

## Inhalt

- Startpose fuer die Initialkalibrierung
- Referenzhaltung fuer Vergleichsmessungen
- optionale Wiederherstellungspose fuer Wartung oder Re-Setup
- `2026-04-23_neutralpose_arm_haengend.md` als erste dokumentierte Wearable-Referenzpose

## Regeln

- jede Pose klar benennen und wiederholbar beschreiben
- wenn moeglich Fotos oder Messnotizen in `docs/` querreferenzieren

## Schnittstellen/Abhaengigkeiten

- richtet sich nach `../README.md` und `../../CALIBRATION_FRAMEWORK.md`
- wird von `../../firmware/esp32_controller/`, `../../dashboard/`, `../../ros2/`, `../../docs/photos/` und `../../docs/measurement_logs/` genutzt
