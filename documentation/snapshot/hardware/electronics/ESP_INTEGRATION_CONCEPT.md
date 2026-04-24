# ESP Integration Concept

Dieses Dokument ordnet die aktuelle Rolle der drei ESP32-Knoten im Prototyp ein.

## Rollen

| Knoten | Rolle | Status |
| --- | --- | --- |
| Controller | Sensorik, Referenz, Greifer-Eingabe, ESP-NOW-Sendung | aktiv |
| Receiver | steuerrelevanter Funk-zu-I2C-Uebergang | aktiv |
| Bridge | rein lesender Debugknoten zu MQTT/Pi | aktiv |

## Aktueller Hardwarestand

- Controller auf Lochraster-/Perfboard
- Greifer-Eingabe aktuell als Potentiometer
- Receiver am Arm als Steuerpfad
- Bridge fuer Dashboard und ROS verifiziert

## Architekturprinzip

Steuerpfad:

```text
Controller -> Receiver -> Arduino -> Servos
```

Debugpfad:

```text
Controller -> Bridge -> MQTT -> Pi -> Dashboard / ROS
```

Diese Trennung bleibt bewusst erhalten.

## Historie

Offene Breadboard- und fruehe Sensorpfade bleiben als Entwicklungsherkunft dokumentiert, sind aber nicht mehr die aktive Integrationsbeschreibung.
