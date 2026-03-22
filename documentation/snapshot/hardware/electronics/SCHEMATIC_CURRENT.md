# Schematic Current

Aktueller Bench-Aufbau — Stand 2026-03-22.
Nur bestaetigte und getestete Verbindungen. Geplante aber noch nicht getestete Pfade sind als `(geplant)` markiert.

## Controller-Seite

```
3.3V ──────────────────────────────────────┐
                                           │
GND ────────────────────────────────────┐  │
                                        │  │
ESP32-S3-WROOM-1-N16R8                  │  │
┌─────────────────────┐                 │  │
│                     │                 │  │
│  GPIO8  (SDA) ──────┼─────────── SDA ─┤  │
│  GPIO9  (SCL) ──────┼─────────── SCL ─┤  ├── PCA9548A (0x70)
│                     │            RST ─┼──┘  │
│                     │     A0/A1/A2 ───┼─────┘(alle GND)
│                     │                 │
│                     │           SD0 ──┼──── SDA ─┐
│                     │           SC0 ──┼──── SCL ─┤── BNO055 #0 (0x29, Kanal 0)
│                     │                 │     VIN ──┤── 3.3V
│                     │                 │     GND ──┘
│                     │                 │
│                     │           SD1 ──┼──── SDA ─┐
│                     │           SC1 ──┼──── SCL ─┤── BNO055 #1 (0x29, Kanal 1)
│                     │                 │     VIN ──┤── 3.3V
│                     │                 │     GND ──┘
│                     │
│  GPIO1  (ADC1) ─────┼──┬── Flex-Sensor-Ende ── 3.3V
│                     │  │
│                     │  └── 10kOhm ── GND
│                     │
│  GPIO4  (geplant) ──┼── LED Oberarm  + Vorwiderstand ── GND
│  GPIO5  (geplant) ──┼── LED Unterarm + Vorwiderstand ── GND
│  GPIO6  (geplant) ──┼── LED Hand/Wrist + Vorwiderstand ── GND
│  GPIO7  (geplant) ──┼── LED COMMS + Vorwiderstand ── GND
│  GPIO10 (geplant) ──┼── LED FAULT + Vorwiderstand ── GND
│  GPIO21 (geplant) ──┼── Buzzer-Treiber ── Buzzer
│                     │
│  USB-C ─────────────┼── PC (Flash / Serial Monitor)
└─────────────────────┘
```

## Receiver-Seite

```
ESP32-S3-WROOM-1-N16R8
┌─────────────────────┐
│                     │
│  WiFi (intern) ─────┼──)) ESP-NOW Unicast von Controller
│                     │
│  GPIO4  (geplant) ──┼── LED LINK  + Vorwiderstand ── GND
│  GPIO5  (geplant) ──┼── LED UART  + Vorwiderstand ── GND
│  GPIO6  (geplant) ──┼── LED FAULT + Vorwiderstand ── GND
│  GPIO15 (geplant) ──┼── UART TX ── Arduino RX
│  GPIO16 (geplant) ──┼── UART RX ── Arduino TX
│  GPIO21 (geplant) ──┼── Buzzer-Treiber ── Buzzer
│                     │
│  USB-C ─────────────┼── PC (Flash / Serial Monitor)
└─────────────────────┘
```

## Datenfluss

```
BNO055 #0 ──┐
             ├── I2C ── PCA9548A ── I2C ── ESP32-S3 (Controller)
BNO055 #1 ──┘                                    │
                                                  │ ADC
                                          Flex-Sensor
                                                  │
                                          ESP-NOW (Unicast)
                                                  │
                                       ESP32-S3 (Receiver)
                                                  │
                                          UART (geplant)
                                                  │
                                            Arduino (geplant)
```

## Paketstatus

- ESP-NOW Paketformat: `ImuPaket v1` (siehe `COMMUNICATION_FRAMEWORK.md`)
- Pruefsumme: XOR, `__attribute__((packed))`
- Frische-Check: Frame-Zaehler
- Sendeintervall: 50ms (20Hz)
