# hardware/mechanik

## Zweck

Dieser Ordner dokumentiert die mechanische Integration von Sensoren, Elektronik und Roboterarm.

## Aktueller Stand

Die mechanische Dokumentation ist vorbereitet, reale Halterungen und Montagevarianten folgen spaeter.

## Inhalt

- IMU-Befestigung am menschlichen Arm
- `WEARABLE_SENSOR_CONCEPT.md` fuer das allgemeine Tragekonzept und die Zielpositionen
- `WEARABLE_PROTOTYPE_V1.md` fuer den ersten strap-basierten Prototyp
- `WEARABLE_FINAL_WEAR_SYSTEM.md` fuer den spaeteren stabilen Betriebstyp
- Halterungen fuer Controller-Elektronik
- Montage von Receiver und Arduino am Roboterarm
- Hinweise zu Kabelentlastung und Bewegungsfreiheit

## Regeln

- jede mechanische Loesung mit Einsatzzweck und Grenzen beschreiben
- Befestigungen, die Sensororientierung beeinflussen, immer mit Kalibrierhinweisen verknuepfen
- mechanische Konzept- und Entscheidungsdokumente muessen die verwendeten Quellen in einem Abschnitt `Recherchequellen` sammeln

## Schnittstellen/Abhaengigkeiten

- referenziert `calibration/imu/` fuer Sensororientierungen
- arbeitet mit `../electronics/` fuer Traegermodule, Gehaeuse und Kabelfuehrung zusammen
- wird durch `docs/photos/` und `docs/measurement_logs/` ergaenzt
- arbeitet mit `docs/templates/SENSOR_MOUNTING_RECORD_TEMPLATE.md` fuer spaetere Nachweise
