# Pin Mapping Confirmed

Aktiver Pinstand des Prototyps, Stand 2026-04-24.

## Controller

| Pin | Rolle |
| --- | --- |
| GPIO1 | Potentiometer fuer Greifer-Eingabe |
| GPIO4 | LED Oberarm / S2 |
| GPIO5 | LED Unterarm / S1 |
| GPIO6 | LED Hand / S0 |
| GPIO7 | COMMS LED |
| GPIO8 | I2C SDA |
| GPIO9 | I2C SCL |
| GPIO10 | FAULT LED |
| GPIO21 | Notaus-Button |
| GPIO48 | RGB |

## Receiver

| Pin | Rolle |
| --- | --- |
| GPIO4 | I2C LED |
| GPIO5 | ESP-NOW / LINK LED |
| GPIO13 | I2C SDA zum Arduino |
| GPIO14 | I2C SCL zum Arduino |
| GPIO48 | RGB |

## Bridge

| Pin | Rolle |
| --- | --- |
| GPIO4 | WiFi LED |
| GPIO5 | ESP-NOW LED |
| GPIO7 | MQTT LED |
| GPIO48 | RGB |

## Hinweis zur Historie

Fruehere Dokumente koennen noch einen Flex-Sensor oder einen reinen Breadboard-Stand beschreiben.
Fuer aktive Arbeit an Prototyp, Firmware, Dashboard und ROS gilt dieses Dokument als Referenz.
