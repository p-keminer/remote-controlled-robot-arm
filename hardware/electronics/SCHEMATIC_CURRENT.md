# Schematic Current

Aktueller Elektronikstand des Prototyps, Stand 2026-04-24.

## Einordnung

Dieses Dokument beschreibt den aktuellen Arbeitsstand des Projekts.
Historische Breadboard-Varianten bleiben unter `PROTO_BREADBOARD_PLAN.md` dokumentiert, sind aber nicht mehr die aktive Zielbeschreibung.

## Controller

| Signal | Pin | Aktuelle Funktion |
| --- | --- | --- |
| I2C SDA | GPIO8 | PCA9548A und BNO055 |
| I2C SCL | GPIO9 | PCA9548A und BNO055 |
| ADC / Greifer | GPIO1 | Potentiometer als Greifer-Eingabe |
| LED S2 | GPIO4 | Oberarm-Status |
| LED S1 | GPIO5 | Unterarm-Status |
| LED S0 | GPIO6 | Hand-/Wrist-Status |
| COMMS LED | GPIO7 | ESP-NOW-/Kommunikationsstatus |
| FAULT LED | GPIO10 | Fehleranzeige |
| Notaus-Button | GPIO21 | Toggle-Notaus |
| RGB | GPIO48 | Fehler-/Notausanzeige |

Sensorzuordnung:

- Kanal 0 = Hand/Wrist
- Kanal 1 = Unterarm
- Kanal 2 = Oberarm

## Greifer-Eingabe

Der aktuelle Prototyp nutzt ein 10k-Potentiometer aus Robustheitsgruenden.
Die Greifer-Eingabe liegt auf GPIO1.
Im Protokoll bleibt das Feld `f` als Greifer-Prozentwert erhalten.

## Receiver

| Signal | Pin | Aktuelle Funktion |
| --- | --- | --- |
| I2C SDA | GPIO13 | zum Arduino A4 |
| I2C SCL | GPIO14 | zum Arduino A5 |
| LED I2C | GPIO4 | I2C-/Statusanzeige |
| LED LINK | GPIO5 | ESP-NOW-/Linkanzeige |
| RGB | GPIO48 | Fehler-/Notausanzeige |

## Arduino-Armboard

Der Arduino bleibt fuer die Servoausfuehrung zustaendig.
Der Receiver uebergibt `Frame V1` per I2C an den Arduino-Slave `0x42`.

Servozuordnung:

- D9 = Base
- D11 = Shoulder
- D3 = Elbow
- D5 = Wrist
- D6 = Gripper

## Bridge

| Signal | Pin | Aktuelle Funktion |
| --- | --- | --- |
| LED WiFi | GPIO4 | WiFi-Status |
| LED LINK | GPIO5 | ESP-NOW-Status |
| LED MQTT | GPIO7 | MQTT-Status |
| RGB | GPIO48 | Fehler-/Notausanzeige |

Die Bridge bleibt rein lesend und speist den verifizierten Debugpfad zu Pi, Dashboard und ROS.

## Aktive Hardwareform

- Controller: Lochraster-/Perfboard-Prototyp
- Receiver: projektiver Steuerknoten am Arm
- Bridge: separater Debugknoten
- Arm: aufgebauter Prototyp mit gemapptem Twin
