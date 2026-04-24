# Roadmap - dashboard

## Ziel

Das Dashboard soll der schnelle, lesende Zwilling des realen Arms bleiben: Live beobachten, Mapping justieren, Fehler eingrenzen, aber keine Bewegung ausloesen.

## Abgeschlossen

- [x] Robotarm-Tab mit Live-Daten, Statistik, 3D-Twin und Debug-Konsole
- [x] MQTT-WebSocket-Pfad ueber Mosquitto und Nginx
- [x] 3D-Szene fuer den Wandaufbau
- [x] Gelenkmapping fuer Basis, Schulter, Ellenbogen, Wrist und Greifer
- [x] Pi-Deployment und Live-Abgleich gegen den realen Prototyp
- [x] Debug-Bridge zusammen mit ROS und MQTT end-to-end verifiziert
- [x] Kollisionssolver fuer das aktuelle Tuning bewusst deaktiviert

## Offen

- [ ] Schulter-, Ellenbogen- und Wrist-Mapping weiter feinziehen, bis Dashboard und ROS gleich reagieren
- [ ] Latenz und Bildwiederholrate am Pi unter laengerem Live-Betrieb dokumentieren
- [ ] klare dated Nachweise fuer weitere Mapping-Meilensteine nach `docs/measurement_logs/` nachziehen
- [ ] optional spaeter eine explizite reine Kollisionsvisualisierung ergaenzen, ohne in das Gelenkmapping einzugreifen
