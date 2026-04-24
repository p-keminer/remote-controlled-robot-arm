# System Frameworks

Dieses Dokument beschreibt die verbindlichen Subsysteme und ihre Verantwortungen.

## Uebersicht

Das Projekt ist in folgende feste Bereiche gegliedert:

- `preparation/` fuer Toolchain, Bench und Inbetriebnahmebereitschaft
- `security/` fuer Bedrohungsmodell, Kommunikationshaertung und Provisioning-Regeln
- `firmware/esp32_controller/` fuer Sensorerfassung, Vorverarbeitung und Sendevorbereitung
- `firmware/esp32_receiver/` fuer Funkempfang, Validierung und I2C-Uebergabe an den Arduino
- `firmware/esp32_bridge/` fuer die beobachtende Debug-Bridge
- `firmware/arduino_arm/` fuer Servoansteuerung, Limits, Rampen und Fehlerverhalten
- `hardware/` fuer Schaltplaene, Verdrahtung, Aufbauablauf, Bringup und Stueckliste
- `calibration/` fuer Referenzdaten, Grenzen und Twin-/Servo-Mappingwissen
- `dashboard/` fuer den browserbasierten Digital Twin und MQTT-Debug-Views
- `ros2/` fuer den ROS-2-Digital-Twin, Replay- und Monitoring-Pfad
- `tests/` fuer Bench-, Security-, Integrations-, Latenz- und Safety-Nachweise
- `docs/` fuer Messprotokolle, Bilder, dated Logs und Templates

## Controller-Framework

- liest drei IMUs und die aktuelle Greifer-Eingabe in definierter Reihenfolge
- fuehrt Vorverarbeitung, Referenzbezug und Paketaufbereitung durch
- erzeugt keine direkte Servoausgabe

## Receiver-Framework

- kapselt den `ESP-NOW`-Empfang
- validiert Paketinhalte, Zeitverhalten und Integritaet
- uebersetzt Funkframes in ein I2C-Frame fuer den Arduino

## Bridge-Framework

- liest denselben ESP-NOW-Datenstrom beobachtend mit
- publiziert MQTT fuer Dashboard, MCP und ROS 2
- darf keine Steuerbefehle in den Bewegungsweg einspeisen

## Arduino-Framework

- haelt die direkte Verbindung zur Servo-Hardware
- setzt Gelenklimits, Neutralpositionen und Rampen durch
- reagiert definiert auf Datenverlust oder ungueltige Frames

## Kalibrierungs-Framework

- trennt IMU-Referenzen, Greifer-Eingaben, Servo-Limits und Referenzposen
- behandelt `calibration/flex_sensor/` als historischen Bench-Pfad
- fuehrt den aktuellen Potentiometer-Arbeitsstand unter `calibration/gripper_input/`

## Twin-Framework

- `dashboard/` visualisiert MQTT-Daten live im Browser und bildet den aktuellen Mapping-Stand als Three.js-Szene ab
- `ros2/` bildet denselben Gelenkstand in RViz, Recorder, Replay, Plot und Live-Monitor ab
- beide Twin-Pfade sind beobachtend und kein Ersatz fuer reale Safety-Freigaben
