# Dashboard Concept

Dieses Dokument beschreibt die aktuelle Rolle des Dashboards als digitaler Zwilling des Roboterarm-Prototyps.

## Einordnung

Das Dashboard ist ein Entwicklungswerkzeug auf dem Pi.
Es visualisiert den aktuellen Sensor- und Mappingzustand des Arms, ist aber bewusst nicht Teil des eigentlichen Steuerpfads `Controller -> Receiver -> Arduino -> Servos`.

Der Debugpfad lautet aktuell:

```text
Controller ESP32
  -> ESP-NOW -> Bridge ESP32
  -> WiFi/MQTT -> Mosquitto auf dem Pi
  -> WebSocket -> Browser-Dashboard
```

Zusatzpfade fuer dieselben Daten:

- MQTT-MCP fuer lesendes Debugging
- ROS-Bridge in WSL fuer RViz, Recorder, Replay und Live-Monitor

## Aktueller Funktionsumfang

- Live-Karten fuer die drei IMUs
- Statistik und Kommunikationszustand der Bridge
- 3D-Twin des Wandaufbaus
- Debug-Konsole fuer Rohdaten
- verifiziertes Gelenkmapping vom realen Arm in den Twin

## Twin-Mapping

Der Dashboard-Twin bildet den aktuellen Prototypstand ab:

- Basis aus `S2.pitch`
- Schulter aus `S2.heading`
- Ellenbogen aus relativer Oberarm-/Unterarm-Rotation
- Wrist aus relativer Hand-/Unterarm-Rotation
- Greifer aus `f` als Greifer-Prozentwert

`f` bleibt aus Protokollkompatibilitaet im Payload erhalten, wird aber heute als Potentiometer-basierte Greifer-Eingabe interpretiert.

## Wichtige Projektentscheidung

Der Dashboard-Kollisionssolver ist derzeit absichtlich deaktiviert.
Beim Feintuning hat sich gezeigt, dass die automatische Nachregelung das reale Schulter- und Ellenbogenmapping sichtbar verfaelscht.
Das Dashboard bildet deshalb aktuell das direkte Mapping ab, nicht eine nachtraeglich korrigierte Bewegung.

## MQTT-Topics

```text
robotarm/imu
robotarm/status
robotarm/kalib
robotarm/debug/raw_imu
robotarm/debug/mapping
```

Die Debug-Topics stammen aus dem ROS-/Bridge-Debugpfad und werden fuer Analyse, Recorder und Live-Monitor genutzt.

## `robotarm/imu` Payload

```json
{
  "z": 12345,
  "s": [
    {"h": 92.3, "r": 27.1, "p": 17.5},
    {"h": 359.9, "r": -46.1, "p": 4.2},
    {"h": 119.4, "r": 21.9, "p": 149.3}
  ],
  "k": [
    {"s": 3, "g": 3, "a": 3, "m": 3},
    {"s": 0, "g": 3, "a": 1, "m": 2},
    {"s": 0, "g": 3, "a": 1, "m": 2}
  ],
  "f": 42.5,
  "v": 4
}
```

Schluessel:

- `z` = Zaehler
- `s` = Sensoren
- `k` = Kalibrierstatus
- `f` = Greifer-Prozent
- `v` = Protokollversion

## Abgrenzung zu Safety und Security

- Das Dashboard darf nichts in den Steuerpfad zurueckschreiben.
- WiFi/MQTT ist ein Debug- und Beobachtungspfad, kein Freigabepfad fuer Bewegung.
- Der erfolgreiche Twin beweist ein gutes Mapping, aber keine Safety- oder Produktivfreigabe.

## Deployment-Kontext

Das Dashboard erweitert das bestehende IoT Control Center auf dem Pi.
Es wird kein separater Webserver fuer den Roboterarm betrieben.

## Verwandte Dokumente

- `README.md` fuer den aktuellen Arbeitsstand
- `ROADMAP.md` fuer offene Dashboard-Themen
- `../COMMUNICATION_FRAMEWORK.md` fuer das Protokoll
- `../calibration/reference_poses/` fuer die Neutralpose
- `../docs/measurement_logs/2026-04-24_debug_bridge_und_twin_mapping.md` fuer den verifizierten Debug- und Mappingstand
