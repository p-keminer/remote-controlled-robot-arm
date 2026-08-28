# Aktueller Schaltungsstand

Diese Datei bleibt als Kompatibilitaetsziel fuer historische Quellcode-Verweise
erhalten. Die kanonische und aktuelle Verdrahtung steht in
[`docs/HARDWARE.md`](../../docs/HARDWARE.md); Betriebsgrenzen stehen in
[`SAFETY.md`](../../SAFETY.md).

Aktiver Bewegungsweg:

```text
Controller --ESP-NOW--> Receiver --I2C--> Arduino --> Servos
```

Separater Diagnoseweg:

```text
Controller --ESP-NOW--> Bridge --WiFi/MQTT--> Dashboard / MCP / ROS 2
```

Der archivierte UART-Pfad verwendete ESP32 `GPIO15` als TX und Arduino `D2`
als SoftwareSerial-RX. Er liegt nur noch unter
[`firmware/archive/uart/`](../../firmware/archive/uart/) und ist kein aktiver
Build- oder Flashpfad.

Die Bridge gehoert nicht zum Bewegungsweg. Der MCP-Server besitzt jedoch ein
Publish-Werkzeug und ist daher nur bei wirksamer Broker-ACL als read-only zu
behandeln; siehe [`SECURITY.md`](../../SECURITY.md).
