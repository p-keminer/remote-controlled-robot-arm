# Schematic Current

Aktueller Bench-Aufbau — Stand 2026-03-28.
Nur bestaetigte und getestete Verbindungen. Geplante aber noch nicht getestete Pfade sind als `(geplant)` markiert.

**LED-Schema:** Invertiert — aus = OK, an = Problem. RGB auf GPIO48 als FAULT auf allen ESPs.

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
│  GPIO4  ────────────┼── LED Gruen (Hand S2)    + 100 Ohm ── GND  [leuchtet bei Problem]
│  GPIO5  ────────────┼── LED Gelb (Unterarm S1) + 100 Ohm ── GND  [leuchtet bei Problem]
│  GPIO6  ────────────┼── LED Rot  (Oberarm S0)  + 100 Ohm ── GND  [leuchtet bei Problem]
│  GPIO7  ────────────┼── LED Blau (COMMS)       + 100 Ohm ── GND  [leuchtet bei Problem]
│  GPIO10 ────────────┼── LED Weiss (FAULT)      + 100 Ohm ── GND  [leuchtet bei Fehler]
│  GPIO21 ────────────┼── Toggle-Button ── GND  (Notaus, INPUT_PULLUP, Toggle-Notaus)
│  GPIO48 ────────────┼── LED RGB onboard (FAULT/NOTAUS)             [rot/orange blinkt]
│                     │
│  WiFi (intern) ─────┼──)) ESP-NOW Kanal 1 → Receiver + Bridge
│                     │
│  USB-C ─────────────┼── PC (Flash / Serial Monitor)
└─────────────────────┘
```

## Receiver-Seite

```
ESP32-S3-WROOM-1-N16R8
┌─────────────────────┐
│                     │
│  WiFi (intern) ─────┼──)) ESP-NOW Kanal 1 ← Controller
│                     │
│  GPIO4  ────────────┼── LED Gruen (UART)       + 100 Ohm ── GND  [reserviert, spaeter]
│  GPIO5  ────────────┼── LED Blau (ESP-NOW)     + 100 Ohm ── GND  [blinkt bei Timeout]
│  GPIO48 ────────────┼── LED RGB onboard (FAULT)                   [rot blinkt bei Fehler]
│  GPIO15 (geplant) ──┼── UART TX ── Arduino RX
│  GPIO16 (geplant) ──┼── UART RX ── Arduino TX
│                     │
│  USB-C ─────────────┼── PC (Flash / Serial Monitor)
└─────────────────────┘
```

## Bridge-Seite (Entwicklungswerkzeug)

```
ESP32-S3-WROOM-1-N16R8
┌─────────────────────┐
│                     │
│  WiFi (intern, STA) ┼──)) WiFi zu Router auf Kanal 1
│  ESP-NOW (intern) ──┼──)) ESP-NOW Kanal 1 ← Controller
│                     │
│  GPIO4  ────────────┼── LED Gruen (WiFi)       + 100 Ohm ── GND  [blinkt wenn getrennt]
│  GPIO5  ────────────┼── LED Blau (ESP-NOW)     + 100 Ohm ── GND  [blinkt bei Timeout]
│  GPIO7  ────────────┼── LED Weiss (MQTT)       + 100 Ohm ── GND  [blinkt wenn getrennt]
│  GPIO48 ────────────┼── LED RGB onboard (FAULT)                   [rot blinkt bei Fehler]
│                     │
│  USB-C ─────────────┼── PC (Flash / ArduinoOTA)
└─────────────────────┘

         │ MQTT (WiFi)
         ▼
    Mosquitto (Pi Zero 2W, Port 1883)
         │
    ┌────┼────┐
    │    │    │
 Dashboard  MCP   api.php
 (Browser)  (Claude)
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
                                             ┌── GPIO4  Gruen  (S2 Problem)
                                             ├── GPIO5  Gelb   (S1 Problem)
                                             ├── GPIO6  Rot    (S0 Problem)
                                             ├── GPIO7  Blau   (COMMS Problem)
                                             ├── GPIO10 Weiss  (FAULT LED)
                                             ├── GPIO48 RGB    (FAULT)
                                             │
                                       ESP-NOW (Kanal 1, Unicast, ImuPaket v4)
                                             │
                                    ┌────────┴────────┐
                                    │                 │
                           ESP32-S3 (Receiver)   ESP32-S3 (Bridge)
                                    │                 │
                     ┌── GPIO4  Gruen  (UART)    ┌── GPIO4  Gruen  (WiFi)
                     ├── GPIO5  Blau   (ESP-NOW) ├── GPIO5  Blau   (ESP-NOW)
                     ├── GPIO48 RGB    (FAULT)   ├── GPIO7  Weiss  (MQTT)
                     │                           ├── GPIO48 RGB    (FAULT)
                UART (geplant)                   │
                     │                      MQTT (WiFi)
                Arduino (geplant)                │
                                         Mosquitto (Pi)
```

## WiFi-Kanal-Alignment

Alle drei ESPs muessen auf dem gleichen WiFi-Kanal laufen fuer ESP-NOW/WiFi-Koexistenz:

- **Router:** Kanal 1
- **Controller:** ESP-NOW auf Kanal 1 (per `esp_wifi_set_channel(1)`)
- **Receiver:** ESP-NOW auf Kanal 1 (per `esp_wifi_set_channel(1)`)
- **Bridge:** WiFi STA + ESP-NOW Empfang auf Kanal 1 (automatisch durch WiFi-Verbindung zum Router)

## Paketstatus

- ESP-NOW Paketformat: `ImuPaket v4` (siehe `COMMUNICATION_FRAMEWORK.md`)
- Inhalt: 3x Euler-Daten (H/R/P), 3x Kalibrierungsstatus (S/G/A/M), Flex-Prozent, Flags (Bit 0 = Notaus)
- Pruefsumme: XOR, `__attribute__((packed))`
- Frische-Check: Frame-Zaehler
- Sendeintervall: 50ms (20Hz) — aktuell ~1-2 PPS durch Multi-Peer Timing
- Kalibrierungsoffsets: persistent im ESP32-NVS, automatisches Laden beim Boot

## LED-Verhalten (invertiert: aus = OK, an = Problem)

### Controller
- IMU-LEDs (Gruen/Gelb/Rot): blinken wenn Sensor NICHT bereit oder NICHT kalibriert
- COMMS (Blau): blinkt wenn ESP-NOW Send fehlschlaegt
- FAULT (RGB orange): blinkt bei Notaus (hoechste Prioritaet)
- FAULT (RGB rot): blinkt bei IMU-Ausfall oder Flex-Sensor-Ausfall (Live-Erkennung im Betrieb)
- Beim Boot: alle LEDs blitzen kurz auf (Starttest)
- Im Kalibrierungsmodus (CAL0/1/2): jeweilige Sensor-LED blinkt

### Receiver
- UART (Gruen): reserviert fuer spaetere UART-Weiterleitung
- ESP-NOW (Blau): blinkt bei Empfangs-Timeout (>2s)
- NOTAUS (RGB orange): blinkt bei empfangenem Notaus-Flag (hoechste Prioritaet)
- FAULT (RGB rot): blinkt bei Validierungsfehler oder Timeout
- Beim Boot: alle LEDs blitzen kurz auf (Starttest)

### Bridge
- WiFi (Gruen): blinkt wenn WiFi getrennt
- ESP-NOW (Blau): blinkt wenn kein Paket seit 2s
- MQTT (Weiss): blinkt wenn MQTT getrennt
- NOTAUS (RGB orange): blinkt bei empfangenem Notaus-Flag (hoechste Prioritaet)
- FAULT (RGB): orange blinkend bei Notaus (hoechste Prio), rot blinkend bei Fehler, aus wenn OK
- Beim Boot: alle LEDs blitzen kurz auf (Starttest)

## Sensorausfallerkennung (Live)

- IMU: `getSystemStatus()` wird jeden Loop-Durchgang geprueft; Ausfall wird sofort erkannt und gemeldet
- Flex-Sensor: ADC-Wert wird auf plausiblen Bereich (200-3800) geprueft
- Wiederherstellung: Sensoren werden automatisch re-initialisiert wenn sie wieder erreichbar sind
