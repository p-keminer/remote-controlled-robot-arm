# Globale Roadmap

Dieses Dokument beschreibt den uebergeordneten Ablaufplan fuer das gesamte Roboterarm-Projekt.

## Phase 1 - Dokumentationsfundament

- [x] Root-Dokumente, Frameworks und Bereichs-READMEs aufgebaut
- [x] `documentation/` als generierten Snapshot eingefuehrt
- [ ] Root-, Bereichs- und Nachweisdoku dauerhaft synchron halten

## Phase 2 - Security-Grundlage

- [x] `security/` als eigenen Bereich eingefuehrt
- [x] Advisory-Lage fuer `ESP-NOW` und Debug-Pfade dokumentiert
- [ ] Produktivfreigabe fuer reale Stack-Basis, Session-Bezug und Authentisierungstag nachziehen

## Phase 3 - Vorbereitung und Toolchain

- [x] Arduino IDE als Hauptumgebung und PlatformIO als Fallback dokumentiert
- [x] Custom-Board `esp32:esp32:robotic_arm_s3n16r8` als verbindlichen Flash-Pfad festgezogen
- [ ] lokale Ist-Staende und committed Zielbasis weiter sauber trennen

## Phase 4 - Hardware und Prototyp

- [x] Adeept-Basis, Elektronik- und Bringup-Doku aufgebaut
- [x] Controller auf Lochraster/Perfboard ueberfuehrt
- [x] aktueller Arm-Prototyp mechanisch fertig aufgebaut
- [ ] Stock-Baseline und Learning-Mode des Originalkits sauber nachweisen
- [ ] aktuelle Verkabelungs-, Assembly- und BOM-Nachweise weiter nachziehen

## Phase 5 - Sensorik und Kommunikation

- [x] drei BNO055 ueber PCA9548A validiert
- [x] `ImuPaket v4`, Notaus, Multi-Peer und Bridge-Pfad validiert
- [x] Receiver → Arduino per I2C validiert
- [x] Greifer-Eingabe auf Potentiometer aus Robustheitsgruenden umgestellt
- [ ] Security-Uplift fuer den realen Produktivpfad aufsetzen

## Phase 6 - Kalibrierung und Digital Twin

- [x] Referenzpose fuer den herunterhaengenden Arm dokumentiert
- [x] Dashboard- und ROS-Twin auf denselben Gelenkstand gemappt
- [x] ROS-Recorder, Replay, Plot und Live-Monitor aufgebaut
- [ ] Mapping kontrolliert von der Simulationsfreigabe in die reale Armfreigabe ueberfuehren

## Phase 7 - Reale Armfreigabe

- [ ] Safety-Faelle, Timeout, Limits und Notaus fuer reale Bewegungen formal nachweisen
- [ ] reale Bewegungsfreigabe des Arms auf Basis des bestaetigten Twin-Stands dokumentieren
- [ ] Dauerlauf-, Latenz- und Integrationsnachweise systematisch sammeln

## Phase 8 - Zukunftsausbau

- [ ] mobile Plattform, Controller-/Dashboard-Ausbau und Kamerasicht nur als echte Zukunftsthemen weiterfuehren
- [ ] Aktuatorabstraktion spaeter als eigenen Architekturpfad ausarbeiten
