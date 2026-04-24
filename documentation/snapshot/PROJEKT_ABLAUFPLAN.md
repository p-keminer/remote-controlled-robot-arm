# Projekt Ablaufplan

Dieses Dokument beschreibt den projektmanagementorientierten roten Faden fuer das gesamte Roboterarm-Projekt.

## Historischer Startpunkt und aktueller Arbeitsstand

Der historische Startpunkt war ein dokumentationsgetriebener Bench-Aufbau mit IMUs, ESP-NOW und Flex-Sensor-Prototyp.
Der aktuelle operative Stand liegt inzwischen deutlich weiter:

- der Controller ist auf Lochraster/Perfboard ueberfuehrt
- der aktuelle Arm-Prototyp ist mechanisch fertig aufgebaut
- die Greifer-Eingabe laeuft aktuell ueber ein Potentiometer aus Robustheitsgruenden
- Dashboard und ROS 2 bilden denselben Gelenkstand als Digital Twin ab
- die Debug-Bridge Controller → ESP-NOW → Bridge → MQTT → Pi ist end-to-end verifiziert

Die reale Safety- und Security-Freigabe des Bewegungswegs bleibt davon bewusst getrennt.

## Reihenfolge der aktiven Projektarbeit

### 1. Root- und Bereichsdoku synchron halten

- `README.md`, `ROADMAP.md`, `PROJEKT_FORTSCHRITT.md` und Bereichs-READMEs muessen denselben Projektstand abbilden
- nach groesseren Schritten immer `bash ./scripts/update_docs.sh`

### 2. Toolchain, Flash-Regeln und lokale Daten diszipliniert halten

- Arduino IDE bleibt Hauptpfad
- das Custom-Board `esp32:esp32:robotic_arm_s3n16r8` ist verpflichtend
- lokale Peer-, WiFi- und MQTT-Daten bleiben gitignoriert

### 3. Hardware- und Nachweisbasis weiter vervollstaendigen

- Stock-Baseline des Originalkits sauber dokumentieren
- aktuelle Perfboard-, Assembly- und Wiring-Nachweise dated in `docs/` fortschreiben
- Vendor-Stand und Projektabweichung sichtbar getrennt halten

### 4. Digital Twin als Referenzpfad nutzen

- Dashboard und ROS 2 bleiben der beobachtende Referenzpfad fuer das Gelenkmapping
- Referenzpose, Segmentzuordnung und Greifer-Arbeitsstand werden aus `calibration/` gespeist
- Recorder, Replay, Plot und Live-Monitor dienen als Debug-Nachweis

### 5. Reale Bewegungsfreigabe erst danach

- Security-Uplift, Session-/Auth-Pfad und Safety-Faelle muessen vor realem Freigabebetrieb sichtbar dokumentiert sein
- erst danach wird der bestaetigte Twin-Stand kontrolliert in reale Armbewegung ueberfuehrt

### 6. Zukunftsthemen sauber auslagern

- mobile Plattform, Kamera- und Teleoperationsideen bleiben in `future/`
- bereits gebaute Dashboard-/ROS-Grundlagen gelten nicht mehr als Zukunftsthema, sondern als aktiver Ist-Stand
