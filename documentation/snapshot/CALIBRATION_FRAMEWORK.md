# Calibration Framework

Dieses Dokument beschreibt die verbindliche Kalibrierungsstrategie fuer Sensoren, Gelenkmapping und Referenzdaten.

## Ziel

Kalibrierung soll reproduzierbar machen, wie aus Koerperhaltung und Sensorwerten stabile Servo-Zielwinkel entstehen.

## Teilbereiche

- `calibration/imu/` fuer Sensorzuordnung, Achsrichtung und Nullpunktlogik
- `calibration/flex_sensor/` fuer ADC-Bereiche, Kennlinien und Greifermapping
- `calibration/servo_limits/` fuer mechanische und softwareseitige Winkelgrenzen
- `calibration/reference_poses/` fuer Start- und Referenzhaltungen

## Grundregeln

- Rohwerte, Referenzdaten und daraus abgeleitete Mappingregeln werden getrennt dokumentiert.
- Jede Achsinvertierung oder Sensorrotation muss begruendet und auffindbar sein.
- Referenzhaltungen muessen klar benannt, wiederholbar und fotografisch oder textlich nachvollziehbar beschrieben sein.
- Kalibrierungsannahmen duerfen nicht stillschweigend nur im Code hinterlegt werden.

## IMU-Kalibrierung

- Sensoren werden segmentbezogen dokumentiert, zum Beispiel Oberarm, Unterarm und Hand.
- Nullpunkt und Koerperreferenz werden als definierte Pose behandelt.
- Relative Winkel haben Vorrang vor isolierten Absolutwerten, wenn dadurch die Bedienung robuster wird.

## Flex-Sensor-Kalibrierung

- gestreckte und maximal gebeugte Fingerhaltung bilden die Mindest- und Maximalreferenz
- Messfenster, Rauschen und moegliche Totzonen werden nachvollziehbar beschrieben

## Servo-Limits

- Limits werden aus mechanischer Realitaet abgeleitet und dann softwareseitig abgesichert
- Neutralposition und sichere Rueckfallwerte werden separat von Komfort- oder Bewegungszielen dokumentiert

## Persistenz

- falls Kalibrierdaten spaeter gespeichert werden, muss die Speicherstrategie dokumentiert werden
- bis dahin bleiben Referenzwerte als versionierte Dokumente oder klar benannte Datendateien im Projekt nachvollziehbar
