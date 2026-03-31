# calibration/reference_poses

## Zweck

Dieser Ordner enthaelt definierte Koerper- und Roboterposen fuer Start, Re-Kalibrierung und Vergleichsmessungen.

## Aktueller Stand

Die Struktur ist vorbereitet, konkrete Referenzposen werden spaeter dokumentiert.

## Inhalt

- Startpose fuer die Initialkalibrierung
- Referenzhaltung fuer Vergleichsmessungen
- optionale Wiederherstellungspose fuer Wartung oder Re-Setup

## Regeln

- jede Pose klar benennen und wiederholbar beschreiben
- wenn moeglich Fotos oder Messnotizen in `docs/` querreferenzieren

## Schnittstellen/Abhaengigkeiten

- richtet sich nach `../README.md` und `../../CALIBRATION_FRAMEWORK.md`
- wird von `../../firmware/esp32_controller/`, `../../docs/photos/` und `../../docs/measurement_logs/` genutzt
