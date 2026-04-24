# Architektur

Dieses Dokument beschreibt die feste Systemstruktur des Roboterarm-Projekts.

## Zielbild

Das System uebersetzt Koerperbewegungen in sichere Gelenkzielwerte und trennt dabei bewusst zwischen:

- Vorbereitung und Toolchain-Basis
- Wearable-Sensorik und Vorverarbeitung
- lokalem Funkpfad ueber `ESP-NOW`
- Steuerpfad Receiver → Arduino → Servos
- beobachtender Debug-Bridge ueber WiFi/MQTT
- Digital Twin in Dashboard und ROS 2
- Security-, Safety-, Kalibrierungs- und Nachweisfluss

## Systemgrenzen

### Wearable-Seite

- ein Controller-ESP32 liest drei BNO055 ueber PCA9548A
- die Greifer-Eingabe kommt aktuell ueber ein Potentiometer auf `GPIO1`
- die Controller-Firmware erzeugt fachliche Zielgroessen, aber keine direkte Servo-PWM

### Steuerpfad

- `ESP-NOW` ist der lokale Funkpfad zwischen Controller und Receiver
- der Receiver validiert Frames und uebersetzt sie in das I2C-Frame V1
- das Adeept-Arduino-Board setzt Zielwerte unter Limits, Rampen und Neutralverhalten um
- dieser Pfad bleibt der sicherheitskritische Bewegungsweg

### Debug- und Twin-Pfad

- die Bridge empfaengt denselben ESP-NOW-Datenstrom wie der Receiver als zweiter Peer
- sie publiziert die Daten per WiFi/MQTT an Mosquitto auf dem Pi
- Dashboard, MQTT MCP Server und ROS 2 lesen denselben beobachtenden Datenstrom
- Dashboard und ROS 2 bilden den aktuellen Digital Twin; sie ersetzen keine reale Safety-Freigabe

## Datenfluss

### Steuerpfad

1. IMUs und Greifer-Eingabe liefern Rohdaten an den Controller.
2. Der Controller erzeugt aus Segment- und Referenzbezug ein `ImuPaket v4`.
3. Der Receiver validiert Groesse, Frische und Integritaet.
4. Der Receiver uebergibt ein I2C-Frame V1 an den Arduino.
5. Der Arduino setzt die Gelenkziele unter Beruecksichtigung von Limits und Timeout-Regeln um.

### Debug- und Twin-Pfad

1. Die Bridge empfaengt das gleiche `ImuPaket v4` parallel.
2. Die Bridge publiziert JSON auf `robotarm/imu`, `robotarm/status`, `robotarm/kalib` und `robotarm/ota/log`.
3. Dashboard, MQTT MCP Server und ROS 2 konsumieren diese Topics beobachtend.
4. Dashboard und ROS 2 berechnen daraus den aktuellen Twin-Zustand und Debug-Ansichten.

## Persistente Artefakte

- Vorbereitungs- und Toolchainwissen liegt unter `preparation/`
- Hardware-, Aufbau- und Schaltplanwissen liegt unter `hardware/`
- Kalibrierungs- und Referenzwissen liegt unter `calibration/`
- Laufzeitlogik liegt unter `firmware/`
- Debug-Oberflaeche liegt unter `dashboard/`
- ROS-2-Digital-Twin liegt unter `ros2/`
- Nachweise, Fotos, Messungen und Templates liegen unter `docs/`
- generierte Spiegelstaende liegen unter `documentation/`

## Architekturregeln

- Steuerpfad und Debug-/Twin-Pfad duerfen nicht implizit vermischt werden.
- Kalibrierwissen darf nicht nur als Konstante im Code verbleiben.
- Historische Bench- oder Flex-Sensor-Staende bleiben dokumentiert, gelten aber nicht als aktueller Pfad.
- Eine neue Schnittstelle oder ein neuer Mapping-Stand gilt erst dann als eingefuehrt, wenn Root- und Bereichsdoku denselben Stand abbilden.
