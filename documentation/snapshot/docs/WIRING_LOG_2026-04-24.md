# Wiring Log

## Sitzung

- Datum: 2026-04-24
- Bearbeiter: p-keminer
- Aufbauzustand: aktueller Prototyp- und Twin-Arbeitsstand
- Schaltplanversion: `hardware/electronics/SCHEMATIC_CURRENT.md`

## Verbindung — Controller Sensorik

| Von | Nach | Signal / Zweck | Erwartung | Tatsaechlicher Stand | Hinweis |
| --- | --- | --- | --- | --- | --- |
| ESP32 GPIO8 | PCA9548A SDA | I2C SDA | bidirektional | aktiv | aktueller IMU-Pfad |
| ESP32 GPIO9 | PCA9548A SCL | I2C SCL | Master-Clock | aktiv | aktueller IMU-Pfad |
| PCA9548A CH0 | BNO055 `S0` | I2C | Hand/Wrist | aktiv | `0x29` |
| PCA9548A CH1 | BNO055 `S1` | I2C | Unterarm | aktiv | `0x29` |
| PCA9548A CH2 | BNO055 `S2` | I2C | Oberarm | aktiv | `0x29` |
| ESP32 GPIO1 | Greifer-Potentiometer | ADC1 | Greifer-Eingabe | aktiv | 2-Draht-Aufbau, 10k-Pull-Down bleibt auf Controller |

## Verbindung — Controller / Bridge / Receiver

| Von | Nach | Signal / Zweck | Erwartung | Tatsaechlicher Stand | Hinweis |
| --- | --- | --- | --- | --- | --- |
| Controller | Receiver | `ESP-NOW` | Steuerpfad | aktiv | Kanal 1 |
| Controller | Bridge | `ESP-NOW` | Debug-Pfad | aktiv | Kanal 1, zweiter Peer |
| Bridge | Pi / Mosquitto | WiFi + MQTT | beobachtender Twin-Pfad | aktiv | Topics `robotarm/*` |

## Verbindung — Receiver zu Arduino

| Von | Nach | Signal / Zweck | Erwartung | Tatsaechlicher Stand | Hinweis |
| --- | --- | --- | --- | --- | --- |
| ESP32 GPIO13 | Arduino A4 | I2C SDA | bidirektional | aktiv | Slave `0x42` |
| ESP32 GPIO14 | Arduino A5 | I2C SCL | Master-Clock | aktiv | Frame V1 |
| ESP32 GND | Arduino GND | gemeinsame Masse | 0V Bezug | aktiv | gemeinsamer Bezugspunkt |

## Risiko- oder Fehlerhinweise

- die Greifer-Eingabe ist aktuell bewusst ein Potentiometer-Arbeitsstand; historische Flex-Sensor-Hinweise gelten nicht mehr als aktiver Pfad
- Dashboard und ROS lesen den MQTT-Strom nur beobachtend; der reale Steuerpfad bleibt Receiver → Arduino
- reale Safety-Freigaben bleiben trotz verifiziertem Twin offen

## Nachweise

- `docs/measurement_logs/2026-04-24_debug_bridge_und_twin_mapping.md`
- `hardware/electronics/SCHEMATIC_CURRENT.md`
