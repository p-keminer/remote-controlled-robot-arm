# Communication Security

Dieses Dokument beschreibt die aktuelle Sicherheitsabgrenzung der Kommunikationspfade.

## Aktive Pfade

Steuerpfad:

```text
Controller -> ESP-NOW -> Receiver -> I2C -> Arduino
```

Debugpfad:

```text
Controller -> ESP-NOW -> Bridge -> WiFi/MQTT -> Pi -> Dashboard / ROS
```

## Wichtige Trennung

- Der Steuerpfad ist fuer Bewegung relevant.
- Der Debugpfad ist lesend und dient Beobachtung, Recorder, Replay und Twin.
- Ein verifizierter Debugpfad ist ausdruecklich keine Produktivfreigabe.

## Aktuelle Mindestregeln

- Steuerdaten bleiben im lokalen Funk-/I2C-Pfad.
- WiFi/MQTT bleibt Debug- und Beobachtungsinfrastruktur.
- Pakete muessen validiert werden, bevor sie in Bewegung umgesetzt werden.
- produktive Secrets und Pairingdaten liegen nur in gitignorierten lokalen Dateien.

## Offene Security-Arbeit

- weitere Haertung des Funkpfads
- formale Replay-/Authentisierungsstrategie
- klare Produktivfreigabe erst nach weiteren Nachweisen
