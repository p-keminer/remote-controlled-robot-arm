# Schematic Current

Aktueller Bench-Aufbau — Stand 2026-03-26.
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
│                     │           SC0 ──┼──── SCL ─┤── BNO055 #0 (0x29, Kanal 0) — Oberarm
│                     │                 │     VIN ──┤── 3.3V
│                     │                 │     GND ──┘
│                     │                 │
│                     │           SD1 ──┼──── SDA ─┐
│                     │           SC1 ──┼──── SCL ─┤── BNO055 #1 (0x29, Kanal 1) — Unterarm
│                     │                 │     VIN ──┤── 3.3V
│                     │                 │     GND ──┘
│                     │                 │
│                     │           SD2 ──┼──── SDA ─┐
│                     │           SC2 ──┼──── SCL ─┤── BNO055 #2 (0x29, Kanal 2) — Hand/Wrist
│                     │                 │     VIN ──┤── 3.3V
│                     │                 │     GND ──┘
│                     │
│  GPIO1  (ADC1) ─────┼──┬── Flex-Sensor-Ende ── 3.3V
│                     │  │
│                     │  └── 10kOhm ── GND
│                     │
│  GPIO4  ────────────┼── LED Gruen (Hand)     + 100 Ohm ── GND
│  GPIO5  ────────────┼── LED Gelb (Unterarm)  + 100 Ohm ── GND
│  GPIO6  ────────────┼── LED Rot (Oberarm)    + 100 Ohm ── GND
│  GPIO7  ────────────┼── LED Blau (COMMS)     + 100 Ohm ── GND
│  GPIO10 ────────────┼── LED Weiss (FAULT)    + 100 Ohm ── GND
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
│  GPIO4  ────────────┼── LED Gruen (LINK)     + 100 Ohm ── GND
│  GPIO5  ────────────┼── LED Blau (UART)      + 100 Ohm ── GND
│  GPIO6  ────────────┼── LED Gelb (FAULT)     + 100 Ohm ── GND
│  GPIO15 (geplant) ──┼── UART TX ── Arduino RX
│  GPIO16 (geplant) ──┼── UART RX ── Arduino TX
│                     │
│  USB-C ─────────────┼── PC (Flash / Serial Monitor)
└─────────────────────┘
```

## Datenfluss

```
BNO055 #0 (Oberarm) ──┐
                       │
BNO055 #1 (Unterarm) ─┼── I2C ── PCA9548A ── I2C ── ESP32-S3 (Controller)
                       │                                    │
BNO055 #2 (Hand) ─────┘                                    │ ADC
                                                    Flex-Sensor
                                                            │
                                             ┌── GPIO4  Gruen  (Hand kalibriert)
                                             ├── GPIO5  Gelb   (Unterarm kalibriert)
                                             ├── GPIO6  Rot    (Oberarm kalibriert)
                                             ├── GPIO7  Blau   (COMMS ok)
                                             ├── GPIO10 Weiss  (FAULT)
                                             │
                                       ESP-NOW (Unicast, ImuPaket v3)
                                             │
                                    ESP32-S3 (Receiver)
                                             │
                              ┌── GPIO4  Gruen  (LINK aktiv)
                              ├── GPIO5  Blau   (UART, spaeter)
                              ├── GPIO6  Gelb   (FAULT/Timeout)
                              │
                        UART (geplant)
                              │
                      Arduino (geplant)
```

## Paketstatus

- ESP-NOW Paketformat: `ImuPaket v3` (siehe `COMMUNICATION_FRAMEWORK.md`)
- Inhalt: 3x Euler-Daten (H/R/P), 3x Kalibrierungsstatus (S/G/A/M), Flex-Prozent
- Pruefsumme: XOR, `__attribute__((packed))`
- Frische-Check: Frame-Zaehler
- Sendeintervall: 50ms (20Hz)
- Kalibrierungsoffsets: persistent im ESP32-NVS, automatisches Laden beim Boot

## LED-Verhalten

### Controller
- IMU-LEDs (Gruen/Gelb/Rot): an wenn Sensor bereit UND Gyro kalibriert (>=3)
- COMMS (Blau): an bei erfolgreichem ESP-NOW Senden
- FAULT (Weiss): an bei IMU-Ausfall oder Flex-Sensor-Ausfall (Live-Erkennung im Betrieb)
- Beim Boot: alle 5 LEDs blitzen kurz auf (Starttest)
- Im Kalibrierungsmodus (CAL0/1/2): jeweilige Sensor-LED blinkt

### Receiver
- LINK (Gruen): an bei erfolgreichem Paketempfang
- UART (Blau): reserviert fuer spaetere UART-Weiterleitung
- FAULT (Gelb): an bei Validierungsfehler oder Empfangs-Timeout (>2s)
- Beim Boot: alle 3 LEDs blitzen kurz auf (Starttest)

## Sensorausfallerkennung (Live)

- IMU: `getSystemStatus()` wird jeden Loop-Durchgang geprueft; Ausfall wird sofort erkannt und gemeldet
- Flex-Sensor: ADC-Wert wird auf plausiblen Bereich (200-3800) geprueft
- Wiederherstellung: Sensoren werden automatisch re-initialisiert wenn sie wieder erreichbar sind
