# Roadmap - firmware

## Ziel

Eine getrennte, nachvollziehbare Firmwarebasis fuer Steuerpfad und Debugpfad pflegen.

## Abgeschlossen

- [x] Controller, Receiver, Arduino und Bridge fachlich getrennt
- [x] `ImuPaket v4` mit Multi-Peer-Sendung
- [x] Drei IMUs ueber PCA9548A im Controller
- [x] Greifer-Eingabe auf Potentiometer umgestellt
- [x] I2C-Kette Receiver -> Arduino stabilisiert
- [x] Limits, Slew-Rate und Timeout auf Arduino verankert
- [x] Debug-Bridge zu MQTT, Dashboard und ROS verifiziert

## Offen

- [ ] Security-Uplift des Funkpfads umsetzen
- [ ] formale Fehler- und Safety-Reaktionen fuer Realbetrieb weiter absichern
- [ ] Langzeit- und Ausfalltests der getrennten Pfade dokumentieren
