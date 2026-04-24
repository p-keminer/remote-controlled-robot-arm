# Debug-Bridge und Twin-Mapping 2026-04-24

## Zweck

Dieser Eintrag dokumentiert den verifizierten beobachtenden Datenpfad und den aktuellen Twin-Stand.

## Bestaetigter Pfad

- Controller sendet `ImuPaket v4` per `ESP-NOW`
- Receiver nutzt denselben Strom fuer den realen Steuerpfad
- Bridge liest denselben Strom als zweiter Peer mit
- Bridge publiziert per WiFi/MQTT an Mosquitto
- Dashboard, MQTT MCP Server und ROS 2 konsumieren denselben beobachtenden Datenstrom

## Twin-Stand

- Dashboard nutzt den aktuellen Mapping-Stand in `dashboard/web/js/robotarm_3d.js`
- ROS 2 nutzt den dazu passenden Stand in `ros2/.../mapping.py`
- abgebildete Gelenklogik:
  - Basis ueber `S2.pitch`
  - Schulter ueber `S2.heading`
  - Ellbogen ueber relative Heading-Differenz `S1` zu `S2`
  - Wrist ueber relativen Twist `S0` zu `S1`
  - Greifer ueber das Feld `f`

## Greifer-Arbeitsstand

- aktueller Greifer-Pfad: Potentiometer aus Robustheitsgruenden
- Arbeitsanker: offen `1935`, geschlossen `3020`
- das Protokollfeld `f` bleibt aus Rueckwaertskompatibilitaet erhalten

## Einordnung

- der beobachtende Debug- und Twin-Pfad ist verifiziert
- dieser Stand ist eine belastbare Referenz fuer Doku, Replay und Ursachenanalyse
- reale Safety- und Security-Freigaben des Arms bleiben davon getrennt offen
