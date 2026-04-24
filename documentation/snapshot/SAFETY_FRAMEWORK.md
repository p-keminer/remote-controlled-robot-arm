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

### Manuelle Eingriffe

- der Roboterarm wird im aktiven Betrieb nicht mit der Hand zwangsbewegt
- manuelle Korrekturen oder mechanische Neuorientierungen erfolgen nur in dokumentiert sicherem Zustand
- Herstellerwarnungen zu Handhabung und Stromversorgung werden in Bringup- und Aufbaupfaden mitgezogen

### Notaus (Emergency Stop)

- Der Controller besitzt einen physischen Notaus-Schalter an GPIO21
- Beschaltung: Toggle-Button zwischen GPIO21 und GND, interner Pull-Up aktiv
- Jeder Tastendruck toggelt den Notaus-Zustand. Entprellung 50ms, Zustandswechsel wird geloggt.
- Entprellung: 50ms Software-Debounce
- Signalweg: Controller setzt Bit 0 (`FLAG_NOTAUS`) im `flags`-Feld des ImuPaket v4
- Receiver wertet das Flag aus und sperrt Bewegung bei aktivem Notaus
- Bridge leitet den Notaus-Status per MQTT weiter (`notaus` Feld im JSON)
- LED-Anzeige: RGB orange blinkend (255, 80, 0) auf allen ESPs bei Notaus (hoechste Prioritaet)
- Ohne angeschlossenen Schalter: GPIO21 floating mit Pull-Up = HIGH = Notaus als sicherer Default

### Freigabe

- eine spaetere Totmann- oder Bewegungsfreigabelogik wird als eigenes Sicherheitsfeature behandelt
- bis dahin muss dokumentiert sein, unter welchen Bedingungen das System aktiv Bewegungsbefehle annimmt

### Bridge-Unabhaengigkeit

- ein Ausfall der Bridge, des MQTT-Systems oder des Pi-Dashboards darf den Steuerpfad nicht beeinflussen
- der Receiver muss ohne Bridge funktionieren und Daten an den Arduino weiterleiten koennen
- die Bridge kann keine Steuerbefehle senden und hat keinen Zugriff auf Servobewegung

## Testfreigaben

- neue Bewegungsfunktionen gelten erst dann als belastbar, wenn relevante Sicherheitspruefungen unter `tests/safety/` beschrieben sind
- kritische Messungen und Beobachtungen werden unter `docs/measurement_logs/` festgehalten

## Recherchequellen

- [Read me first.pdf](./official_downloads/extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Read%20me%20first.pdf) fuer die Herstellerwarnung, den Arm im Betrieb nicht per Hand zu zwingen.
