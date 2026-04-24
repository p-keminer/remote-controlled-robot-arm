# tests/security

## Zweck

Dieser Ordner sammelt Security-bezogene Test- und Nachweisfaelle.

## Aktueller Stand

Der Debugpfad ueber WiFi/MQTT ist aktiv und verifiziert, der eigentliche Security-Uplift des Steuerpfads aber noch offen.
Dadurch muessen Security-Tests heute zwei Dinge sauber trennen:

- den aktiven, lesenden Debugpfad
- den spaeter zu haertenden Produktivpfad

## Regeln

- Security bleibt von Safety getrennt.
- Debug-Zugang und Produktivbewegung werden nicht vermischt.
