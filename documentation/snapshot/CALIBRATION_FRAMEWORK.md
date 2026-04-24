# Calibration Framework

Dieses Dokument beschreibt die verbindliche Kalibrierungsstrategie fuer Sensoren, Referenzdaten, Twin-Mapping und Gelenkgrenzen.

## Ziel

Kalibrierung soll reproduzierbar machen, wie aus Koerperhaltung und Eingaben stabile Twin- und spaeter Servo-Zielwerte entstehen.

## Teilbereiche

- `calibration/imu/` fuer Sensorzuordnung, Achsrichtung und Nullpunktlogik
- `calibration/gripper_input/` fuer den aktuellen Potentiometer-Arbeitsstand der Greifer-Eingabe
- `calibration/flex_sensor/` als historischer Prototyp-Pfad des fruehen Bench-Aufbaus
- `calibration/servo_limits/` fuer mechanische und softwareseitige Winkelgrenzen
- `calibration/reference_poses/` fuer Start- und Referenzhaltungen

## Grundregeln

- Rohwerte, Referenzdaten und daraus abgeleitete Mappingregeln werden getrennt dokumentiert.
- Jede Achsinvertierung oder Sensorrotation muss begruendet und auffindbar sein.
- Referenzhaltungen muessen klar benannt, wiederholbar und fotografisch oder textlich nachvollziehbar beschrieben sein.
- Aktuelle Kalibrierungsannahmen duerfen nicht stillschweigend nur im Code hinterlegt werden.

## IMU-Kalibrierung

- Sensoren werden segmentbezogen dokumentiert: `S0 = Hand/Wrist`, `S1 = Unterarm`, `S2 = Oberarm`.
- Relative Winkel haben Vorrang vor isolierten Absolutwerten, wenn dadurch die Bedienung robuster wird.
- Referenzposen muessen sowohl fuer Dashboard als auch fuer ROS nachvollziehbar bleiben.

## Greifer-Eingabe

- Der aktuelle Pfad nutzt ein Potentiometer aus Robustheitsgruenden.
- Die offene und geschlossene Endlage werden als Arbeitsanker dokumentiert und mit dem Protokollfeld `f` auf den Twin gemappt.
- Historische Flex-Sensor-Messungen bleiben zur Nachvollziehbarkeit erhalten, sind aber kein aktiver Kalibrierpfad mehr.

## Servo-Limits

- Limits werden aus mechanischer Realitaet abgeleitet und dann softwareseitig abgesichert.
- Neutralposition und sichere Rueckfallwerte werden separat von Komfort- oder Bewegungszielen dokumentiert.

## Persistenz

- BNO055-Offsets werden im Controller-NVS gespeichert und beim Boot geladen.
- Referenzwerte und Arbeitsanker bleiben zusaetzlich als versionierte Projektdokumente sichtbar.
