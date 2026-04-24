# firmware

## Zweck

Dieser Ordner enthaelt die Laufzeitlogik fuer Controller, Receiver, Servoausfuehrung und Debug-Bridge.

## Aktueller Stand

Die Firmwarebasis ist fuer den aktuellen Prototyp real im Einsatz:

- Controller liest drei IMUs ueber PCA9548A und die Greifer-Eingabe ueber ein Potentiometer auf GPIO1
- Controller sendet `ImuPaket v4` gleichzeitig an Receiver und Bridge
- Receiver uebergibt die validierten Gelenkziele per I2C an das Arduino-Armboard
- Arduino fuehrt Limits, Slew-Rate und Timeout-Neutralverhalten aus
- Bridge speist den verifizierten Debugpfad zu Pi, Dashboard und ROS

Der Debugpfad ist end-to-end bestaetigt.
Offen bleiben der eigentliche Security-Uplift, formale Safety-Freigaben und weitere Langzeithaertung.

## Inhalt

- `esp32_controller/` fuer Wearable-Sensorik, Referenzlogik und Paketbildung
- `esp32_receiver/` fuer den steuerrelevanten ESP-NOW- und I2C-Pfad
- `esp32_bridge/` fuer den rein lesenden Debugpfad per MQTT
- `arduino_arm/` fuer Limits, Rampen und Servoausfuehrung
- `I2C_FRAME_V1.md` fuer das Receiver->Arduino-Frame
- `SERVO_UART_DESIGNENTSCHEIDUNGEN.md` fuer historische Entwurfsentscheidungen
- archivierte Altstaende unter `espnow_*`

## Regeln

- Steuerpfad und Debugpfad bleiben getrennt.
- `f` bleibt im Protokoll als Greifer-Prozentwert erhalten, auch wenn die Eingabe inzwischen vom Potentiometer stammt.
- Sicherheits- oder Protokollaenderungen muessen in den Framework-Dokumenten gespiegelt werden.
- Reale Zugangsdaten, MACs und lokale Secrets gehoeren nur in gitignorierte lokale Dateien.

## Schnittstellen/Abhaengigkeiten

- folgt `../COMMUNICATION_FRAMEWORK.md`
- folgt `../CALIBRATION_FRAMEWORK.md` und `../calibration/`
- folgt `../SAFETY_FRAMEWORK.md` und `../security/`
