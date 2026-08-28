# Firmware

## Aktive Komponenten

| Pfad | Aufgabe |
|---|---|
| `esp32_controller/` | drei BNO055 ueber PCA9548A lesen, Potentiometer erfassen und `ImuPaket v4` senden |
| `esp32_receiver/` | ESP-NOW-Pakete validieren, auf Servo-Zielbytes abbilden und per I2C senden |
| `arduino_arm/` | I2C-Frames validieren, Servolimits und Slew-Rate anwenden |
| `esp32_bridge/` | zweiten ESP-NOW-Peer beobachten und Daten per MQTT an den Debug-/Twin-Stack geben |
| `archive/` | historische ESP-NOW-, UART- und Bring-up-Sketches; nicht aktiv bauen oder flashen |

## Steuerpfad

```text
esp32_controller/esp32_controller.ino
  -> ESP-NOW, ImuPaket v4, Kanal 1
esp32_receiver/esp32_receiver.ino
  -> I2C Frame V1, GPIO13/14, Slave 0x42
arduino_arm/arduino_arm.ino
  -> 5 Servos
```

Der Receiver-Default ist absichtlich konservativ:

```c
RECEIVER_MODE_TEST_SWEEP=1
RECEIVER_MODE_LIVE_I2C=0
RECEIVER_LIVE_ENABLE_BASIS=0
RECEIVER_LIVE_ENABLE_SHOULDER=0
RECEIVER_LIVE_ENABLE_ELBOW=0
RECEIVER_LIVE_ENABLE_WRIST=0
RECEIVER_LIVE_ENABLE_GRIPPER=0
```

Der Live-All-Axes-Build und der reale Prototyptest wurden bewusst ueber
Compile-Flags durchgefuehrt. Die exakten Befehle stehen in
[docs/BUILD.md](../docs/BUILD.md); Testgrenzen stehen in
[docs/TESTING.md](../docs/TESTING.md).

## Debug-/Twin-Pfad

Der Controller kann dasselbe Paket an einen zweiten ESP-NOW-Peer senden. Die
Bridge leitet diesen Beobachtungsstrom ueber WiFi/MQTT zum Raspberry Pi weiter.
Dashboard, MCP und ROS 2 sind aktive Debugwerkzeuge, aber keine Glieder des
Servo-Steuerpfads.

- Bridge-Firmware: `esp32_bridge/esp32_bridge.ino`
- Dashboard und MCP: [dashboard/README.md](../dashboard/README.md)
- ROS-2-Digital-Twin: [ros2/README.md](../ros2/README.md)

## Lokale Konfiguration

Vor einem lokalen Build die Templates kopieren und nur die gitignorierten
lokalen Dateien befuellen:

- Controller: `esp32_controller/peer_config.template.h` -> `peer_config.local.h`
- Bridge: `esp32_bridge/peer_config.template.h` -> `peer_config.local.h`
- Bridge: `esp32_bridge/wifi_config.template.h` -> `wifi_config.local.h`

Keine echten MACs, WLAN-/MQTT-Zugangsdaten oder Schluessel committen.

## Kanonische Dokumentation

- Build und Versionen: [docs/BUILD.md](../docs/BUILD.md)
- Verdrahtung und Servolimits: [docs/HARDWARE.md](../docs/HARDWARE.md)
- Pakete, Topics und Mappings: [docs/PROTOCOL.md](../docs/PROTOCOL.md)
- Referenzpose und Kalibrierung: [calibration/README.md](../calibration/README.md)
- Safety-Grenzen: [SAFETY.md](../SAFETY.md)
- Security-Grenzen: [SECURITY.md](../SECURITY.md)
- Teststatus: [docs/TESTING.md](../docs/TESTING.md)

Die Pfade `I2C_FRAME_V1.md` und `SERVO_UART_DESIGNENTSCHEIDUNGEN.md` bleiben
als kurze Kompatibilitaetsdokumente erhalten, weil aktive und archivierte
Quellkommentare darauf verweisen.
