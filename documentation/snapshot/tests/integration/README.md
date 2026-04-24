# tests/integration

## Zweck

Dieser Ordner beschreibt Ende-zu-Ende-Tests ueber mehrere Teilbereiche.

## Aktueller Stand

Folgende Integrationspfade sind bereits real bestaetigt:

- Controller -> Receiver -> Arduino
- Controller -> Bridge -> MQTT -> Dashboard
- Controller -> Bridge -> MQTT -> ROS

Diese Nachweise machen das System nachvollziehbar, sind aber noch keine Produktivfreigabe.

## Regeln

- Jeder Integrationsfall beschreibt Eingang, Ablauf, erwartetes Verhalten und Beobachtung.
- Debug-/Twin-Erfolg und Safety-Freigabe bleiben getrennt.
