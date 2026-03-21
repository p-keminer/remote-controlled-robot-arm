# Safety Framework

Dieses Dokument definiert die verbindlichen Sicherheitsregeln fuer Bewegungsfreigabe, Grenzwerte und Fehlerverhalten.

## Sicherheitsziele

- keine unkontrollierte Bewegung bei Datenverlust oder ungueltigen Paketen
- keine Ueberschreitung mechanischer Gelenkgrenzen
- nachvollziehbare Erstinbetriebnahme und Wiederanlaufregeln

## Schutzelemente

### Gelenklimits

- jedes Servo erhaelt dokumentierte Mindest- und Hoechstwerte
- Limits werden sowohl in `calibration/servo_limits/` als auch in der Arduino-Ebene gespiegelt

### Neutralposition

- fuer Kommunikations- oder Watchdog-Fehler gibt es definierte sichere Rueckfallwinkel
- Neutralposition ist nicht automatisch identisch mit der ergonomischen Standardpose und wird explizit dokumentiert

### Rampen und Geschwindigkeitsbegrenzung

- neue Zielwerte werden nicht ungebremst auf die Servos gegeben
- Grenzwerte fuer Schrittweite oder Geschwindigkeit muessen nachvollziehbar sein

### Timeout und Watchdog

- Receiver und Arduino brauchen klare Zeitgrenzen fuer gueltige Daten
- bei Zeitueberschreitung wird Bewegung gestoppt oder auf sichere Rueckfallwerte gefahren

### Freigabe

- eine spaetere Totmann- oder Bewegungsfreigabelogik wird als eigenes Sicherheitsfeature behandelt
- bis dahin muss dokumentiert sein, unter welchen Bedingungen das System aktiv Bewegungsbefehle annimmt

## Testfreigaben

- neue Bewegungsfunktionen gelten erst dann als belastbar, wenn relevante Sicherheitspruefungen unter `tests/safety/` beschrieben sind
- kritische Messungen und Beobachtungen werden unter `docs/measurement_logs/` festgehalten
