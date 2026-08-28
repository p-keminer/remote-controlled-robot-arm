# Firmware-Archiv

Dieser Ordner enthaelt historische oder einmalige Diagnose-Sketches. Sie bleiben
als nachvollziehbare Entwicklungsstaende erhalten, gehoeren aber nicht zum
aktiven Build- oder Flashpfad.

## Struktur

- `espnow/`: abgeloeste ESP-NOW-Sender und -Receiver der Versionen 1 und 2
- `uart/`: abgeloeste UART-Kette zwischen ESP32-Receiver und Arduino; Format in
  [`firmware/UART_FRAME_V1.md`](../UART_FRAME_V1.md)
- `bringup/`: einmalige Sensor-, Mux- und Eingabetests fuer den Hardware-Bring-up

Die aktive Firmware liegt weiterhin direkt unter `firmware/esp32_controller/`,
`firmware/esp32_receiver/`, `firmware/esp32_bridge/` und
`firmware/arduino_arm/`. Archiv-Sketches duerfen nicht versehentlich als
aktueller Referenzstand gebaut oder geflasht werden.
