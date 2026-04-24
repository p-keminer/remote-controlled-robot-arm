# Roadmap - firmware/esp32_controller

## Ziel

Den Controller als stabile Quelle fuer IMU-, Referenz- und Greiferdaten pflegen.

## Abgeschlossen

- [x] Triple-IMU-Pfad ueber PCA9548A
- [x] NVS-Kalibrierpersistenz
- [x] Multi-Peer ESP-NOW an Receiver und Bridge
- [x] Greifer-Eingabe auf Potentiometer umgestellt
- [x] serielle Greifer-Diagnose fuer Live-Kalibrierung
- [x] Lochraster-/Perfboard-Prototyp als aktiver Controllerstand

## Offen

- [ ] Referenz- und Nullpunktlogik weiter dokumentieren
- [ ] Ausfall-, Noise- und Langzeithaertung fuer Realbetrieb nachziehen
- [ ] Security-Uplift auf dem Funkpfad einziehen
