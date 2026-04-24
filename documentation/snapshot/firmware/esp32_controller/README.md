# firmware/esp32_controller

## Zweck

Dieser Ordner enthaelt die Wearable-Firmware des Controller-ESP32.

## Aktueller Stand

`esp32_controller.ino` ist der aktive Sketch fuer den aktuellen Prototyp:

- drei BNO055 ueber PCA9548A auf den Kanaelen 0, 1 und 2
- Greifer-Eingabe aktuell als 10k-Potentiometer aus Robustheitsgruenden auf GPIO1
- 2-Draht-Aufbau mit vorhandenem 10k-Pull-Down
- `ImuPaket v4` per ESP-NOW an Receiver und Bridge
- Notaus-Toggle auf GPIO21
- Kalibrierpersistenz im NVS
- Debug-Befehle `FLEXRAW` und `FLEXSTAT` bleiben als serielle Greifer-Diagnose erhalten

Der Controller laeuft auf dem aktuellen Lochraster-/Perfboard-Prototyp und bildet die Basis fuer Dashboard- und ROS-Mapping.

## Wichtige Boardregel

Verbindlicher Build- und Uploadpfad ist das Custom-Board:

```text
esp32:esp32:robotic_arm_s3n16r8
```

Generische `ESP32S3 Dev Module`-Anleitungen gelten hoechstens als historische oder lokale Ausnahme, nicht als Repo-Standard.

## Inhalt

- Sensorinitialisierung fuer die drei IMUs
- Greifer-Eingabe ueber ADC auf GPIO1
- NVS-Kalibrierpersistenz
- Einzelkalibrierung per Serial
- Multi-Peer ESP-NOW an Receiver und Bridge
- Notaus- und LED-Logik

## Regeln

- Der Controller sendet Ziel- und Sensordaten, aber keine direkte Servo-PWM.
- Segmentzuordnung, Referenzpose und Achsinterpretation muessen mit `calibration/` konsistent bleiben.
- Reale MACs, Credentials und lokale Sonderkonfigurationen gehoeren nur in gitignorierte Header.

## Schnittstellen/Abhaengigkeiten

- nutzt `../../CALIBRATION_FRAMEWORK.md`
- nutzt `../../COMMUNICATION_FRAMEWORK.md`
- liefert an `../esp32_receiver/` und `../esp32_bridge/`
