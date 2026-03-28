# Konzeption: IMU-gesteuerter Roboterarm (5 DOF)
**Projekt:** Adeept Robotic Arm Kit – Steuerung per Arm-Bewegung via BNO055 IMUs
**Stand:** 2026-03-07
**Status:** Konzeptphase

> Hinweis: Dieses Dokument bleibt der historische Konzeptursprung des Projekts. Der aktuelle Bench-Stand, die verbindlichen Regeln und die operative Phasenlage werden in `README.md`, `ROADMAP.md`, `PROJEKT_ABLAUFPLAN.md` und `PROJEKT_FORTSCHRITT.md` fortgefuehrt.
> Die konkrete Produktbasis des vorhandenen Kits, die offizielle Package List und die Stromversorgungsfrage werden zusaetzlich in `hardware/ADEEPT_ARM_PRODUCT_BASELINE.md` gepflegt.

---

## 1. Projektziel

Ein 5-DOF-Roboterarm soll intuitiv durch Bewegungen des eigenen Arms gesteuert werden.
Der Benutzer trägt drei BNO055-IMUs (Oberarm, Unterarm, Hand) sowie einen Flex-Sensor
am Finger. Die Bewegungen werden drahtlos auf den Roboterarm übertragen – ohne Controller,
ohne Joystick, durch reine Körperbewegung.

---

## 2. Hardware-Übersicht

### Vorhanden
| Komponente              | Anzahl | Funktion                              |
|-------------------------|--------|---------------------------------------|
| Adeept 5-DOF Roboterarm | 1      | Ziel-Effektor (5 Servos)              |
| BNO055 IMU              | 3–4    | Absolutlage des Arms erfassen         |
| PCA9548A I2C-Mux        | 2      | Mehrere BNO055 an einem I2C-Bus       |
| ESP32                   | 2      | Controller (Wearable) + Receiver (Arm)|
| Flex-Sensor 2.2"        | 1      | Greifersteuerung                      |
| Arduino (Adeept-Board)  | 1      | Servo-PWM-Ansteuerung                 |
| Breadboard / Perfboard  | –      | Prototyp / Endausbau                  |

### Warum BNO055?
Der BNO055 ist für dieses Projekt die optimale Wahl:
- **Integrierte Sensor-Fusion:** Gibt direkt absolute Quaternionen/Euler-Winkel aus.
  Kein Kalman- oder Madgwick-Filter im eigenen Code nötig.
- **Kein Drift:** Magnetometer-Fusion sorgt für stabilen Absolutbezug (anders als
  reine Gyro-/Accel-Kombination wie beim MPU-6050).
- **NDOF-Modus:** 9 Freiheitsgrade, vollautomatisch kalibriert, ~100 Hz Output.

### Warum PCA9548A?
Alle drei BNO055 haben dieselbe I2C-Adresse (0x28 oder 0x29 – nur eine Umschaltung
möglich, nicht drei). Der PCA9548A schaltet 8 I2C-Kanäle einzeln durch:
ESP32 wählt Kanal → liest BNO055 → wählt nächsten Kanal → liest nächsten BNO055.
Mit 2 Stück ist man für Erweiterungen (4. IMU, weitere Sensoren) gerüstet.

---

## 3. Systemarchitektur

```
╔══════════════════════════════╗          ╔══════════════════════════════╗
║  WEARABLE (am Arm getragen)  ║          ║  ARM-EINHEIT (Roboter)       ║
║                              ║          ║                              ║
║  ESP32 [Controller]          ║          ║  ESP32 [Receiver]            ║
║  ├── PCA9548A                ║          ║  └── UART/Serial             ║
║  │    ├── BNO055 #0          ║ ESP-NOW  ║         │                    ║
║  │    │   (Oberarm)          ║ ───────► ║  Arduino [Adeept-Board]      ║
║  │    ├── BNO055 #1          ║  ~2ms    ║  ├── Servo 1 – Basis         ║
║  │    │   (Unterarm)         ║          ║  ├── Servo 2 – Schulter       ║
║  │    └── BNO055 #2          ║          ║  ├── Servo 3 – Ellbogen       ║
║  │        (Hand/Handgelenk)  ║          ║  ├── Servo 4 – Handgelenk    ║
║  └── Flex-Sensor (ADC)       ║          ║  └── Servo 5 – Greifer       ║
║      (Zeigefinger)           ║          ║                              ║
╚══════════════════════════════╝          ╚══════════════════════════════╝
```

### Kommunikation: ESP-NOW
ESP-NOW wurde gegenüber WiFi-UDP und BLE aus folgenden Gründen gewählt:

| Kriterium        | ESP-NOW       | WiFi UDP      | BLE           |
|------------------|---------------|---------------|---------------|
| Latenz           | ~1–3 ms       | ~5–20 ms      | ~20–50 ms     |
| Router nötig?    | Nein          | Ja            | Nein          |
| Reichweite       | ~200 m (LoS)  | ~50 m (WLAN)  | ~10 m         |
| Aufwand          | Gering        | Mittel        | Hoch          |

Für Echtzeit-Motorsteuerung ist ESP-NOW die klare Wahl.
Der Arm funktioniert auch ohne Heimnetz – standalone.

---

## 4. DOF-Mapping: Körper → Roboter

```
Körper-Segment       IMU    Gelenkwinkel → Servo
─────────────────────────────────────────────────────────────────
Oberarm (Yaw)        #0     Basis-Rotation     → Servo 1
Oberarm (Pitch)      #0     Schulter hoch/runter → Servo 2
Unterarm (relativ)   #1     Ellbogen           → Servo 3
Hand (relativ)       #2     Handgelenk         → Servo 4
Finger (Flex)        ADC    Greifer auf/zu     → Servo 5
```

**Schlüsselkonzept – Relative Winkel:**
Nicht der Absolutwinkel jedes IMU wird auf den Servo gemappt,
sondern der **Winkel zwischen zwei benachbarten Körpersegmenten**:

- Ellbogen-Winkel = Pitch(IMU #1) − Pitch(IMU #0)
- Handgelenk-Winkel = Pitch(IMU #2) − Pitch(IMU #1)

Das macht die Steuerung körperpositions-unabhängig: Der Benutzer kann
stehen, sitzen oder den Arm anders halten – der Arm folgt trotzdem
der relativen Gelenkstellung.

**Flex-Sensor:**
ADC-Wert (0–4095 auf ESP32) → lineares Mapping auf Servo-Winkel (z.B. 10°–90°).
Erfordert Kalibrierung (gestreckt / vollständig gebeugt als Referenz).

---

## 5. Datenpfad (Schritt für Schritt)

```
[BNO055 #0,1,2] → I2C → PCA9548A → I2C → ESP32 (Controller)
                                              │
                                    Quaternion → Euler-Winkel
                                    Relative Winkel berechnen
                                    Flex-ADC lesen
                                    Wertebereich clippen (Gelenklimits)
                                              │
                                         ESP-NOW Paket:
                                    {s1, s2, s3, s4, s5} (5x uint8)
                                              │
                                    ESP32 (Receiver)
                                              │
                                         UART → Arduino
                                              │
                               Servo.write(winkel) für alle 5 Servos
```

**Paketformat (6 Bytes):**
```c
typedef struct {
    uint8_t servo[5];   // Winkel 0–180 Grad pro Servo
    uint8_t checksum;   // XOR der 5 Servo-Werte (Integritaetscheck)
} arm_packet_t;
```

---

## 6. Sicherheitskonzept

Fehler im Echtzeit-Steuersystem können Hardware beschädigen oder
Verletzungen verursachen. Daher mehrere Schutzebenen:

### 6.1 Gelenklimits (Software)
Jeder Servo hat definierte Min/Max-Winkel die dem mechanischen
Limit des Adeept-Arms entsprechen. Werte außerhalb werden gekappt.
```c
// Beispiel – muss nach Aufbau kalibriert werden
#define SERVO_BASE_MIN   10
#define SERVO_BASE_MAX  170
```

### 6.2 Geschwindigkeitsbegrenzer (Ramping)
Servos bewegen sich nicht sprunghaft auf den Zielwinkel, sondern
nähern sich schrittweise an. Verhindert Schläge und Motorüberlastung.
```c
// Aktuellen Winkel schrittweise dem Zielwinkel annähern
if (abs(aktuell - ziel) > MAX_SCHRITT) {
    aktuell += (ziel > aktuell) ? MAX_SCHRITT : -MAX_SCHRITT;
}
```

### 6.3 Watchdog / Verbindungsabbruch
Empfängt der Receiver-ESP32 für >200 ms kein Paket:
→ Alle Servos fahren auf definierte Neutralposition.
→ ESP32-Hardware-Watchdog wird aktiviert (verhindert Hänger).

### 6.4 Totmann-Schalter (optional, Phase 5)
Taster am Wearable: Nur bei gedrücktem Taster sendet der Controller.
Loslassen → Arm stoppt sofort. Empfehlenswert für Dauerbetrieb.

---

## 7. Implementierungsplan (Phasen)

### Phase 1 – Sensor-Validierung
**Ziel:** Einzelne BNO055 auslesen, Mux-Umschaltung verstehen.
**Ergebnis:** Serielle Ausgabe von Euler-Winkeln aller 3 IMUs.

- [ ] BNO055 einzeln an ESP32 (ohne Mux) – Bibliothek testen
- [ ] PCA9548A Kanal-Umschaltung implementieren
- [ ] Alle 3 BNO055 gleichzeitig über Mux auslesen
- [ ] Flex-Sensor: ADC-Wert auslesen und kalibrieren

### Phase 2 – Winkel-Mapping & Kalibrierung
**Ziel:** Rohe Quaternionen → sinnvolle Gelenkwinkel.
**Ergebnis:** Stabiles, drift-freies Winkel-Signal pro DOF.

- [ ] Relative Euler-Winkel zwischen IMU-Paaren berechnen
- [ ] Nullpunkt-Kalibrierung (Arm gerade halten = Referenz)
- [ ] Wertebereich testen: Voller Bewegungsbereich → 0°–180°
- [ ] Flex-Sensor: gestreckt/gebeugt kalibrieren

### Phase 3 – ESP-NOW Kommunikation
**Ziel:** Zuverlässige Paket-Übertragung zwischen beiden ESP32.
**Ergebnis:** <5ms Latenz, 0% Paketverlust bei 1m Abstand.

- [ ] ESP-NOW Peer-to-Peer Setup (MAC-Adressen festlegen)
- [ ] Paketstruktur definieren und testen
- [ ] Checksum-Validierung auf Receiver-Seite
- [ ] Verbindungsabbruch-Handling (Watchdog)

### Phase 4 – Arm-Steuerung (Arduino)
**Ziel:** Arduino empfängt Winkel per Serial und steuert Servos.
**Ergebnis:** Arm folgt Handbewegungen in Echtzeit.

- [ ] Arduino Serial-Protokoll definieren (simpel: 6 Bytes)
- [ ] Servo-Bibliothek: alle 5 Servos initialisieren
- [ ] Gelenklimits als Konstanten definieren (manuell ausmessen)
- [ ] Geschwindigkeits-Ramping implementieren
- [ ] Receiver-ESP32 → Arduino UART verbinden und testen

### Phase 5 – Integration & Feinabstimmung
**Ziel:** Gesamtsystem flüssig und intuitiv.

- [ ] Mapping verfeinern (Skalierung, Totzone, Invertierung)
- [ ] Latenz messen (Ziel: Gesamtsystem <30ms End-to-End)
- [ ] Totmann-Schalter nachrüsten (optional)
- [ ] Dauerlauftest (30 min) – Thermik, Drift, Stabilitaet

### Phase 6 – Aufbau auf Perfboard
- [ ] Wearable-Platine: ESP32 + PCA9548A + 3x BNO055 + Flex-Anschluss
- [ ] Befestigung der IMUs am Arm (Klettband-Halterungen)
- [ ] Arm-Einheit: Receiver-ESP32 fest am Adeept-Chassis

---

## 8. Bibliotheken (Arduino IDE / PlatformIO)

| Bibliothek                  | Plattform | Zweck                          |
|-----------------------------|-----------|--------------------------------|
| `Adafruit BNO055`           | ESP32     | BNO055 Sensor auslesen         |
| `Adafruit Unified Sensor`   | ESP32     | Abhängigkeit von obiger Lib    |
| `esp_now.h`                 | ESP32     | ESP-NOW Kommunikation (built-in)|
| `Wire.h`                    | ESP32     | I2C Bus                        |
| `Servo.h`                   | Arduino   | Servo-PWM-Steuerung            |

---

## 9. Offene Fragen / Entscheidungen

1. **Wie viele IMUs werden tatsächlich benötigt?**
   Mit 3 IMUs (Oberarm, Unterarm, Hand) sind alle 5 DOF abgedeckt.
   Ein 4. IMU könnte Redundanz liefern oder einen zweiten Handgelenk-DOF
   abdecken – zunächst mit 3 starten.

2. **Nullpunkt-Kalibrierung beim Start:**
   Soll der Arm bei jedem Start neu kalibriert werden (Arm 5s gerade halten)?
   Oder feste Referenz-Quaternion im Flash speichern?
   → Empfehlung: Kalibrierungs-Taste am Wearable für On-Demand-Reset.

3. **Invertierung einzelner Achsen:**
   Je nach IMU-Montagerichtung müssen manche Achsen invertiert werden.
   Wird in Phase 2 empirisch ermittelt.

4. **Receiver-ESP32 ↔ Arduino Protokoll:**
   UART (einfach) vs. I2C (ESP32 als Master). UART bevorzugt –
   kein Adresskonflikt, einfaches Debugging per Serial Monitor.

---

## 10. Verzeichnisstruktur

```
robotic-arm/
├── KONZEPTION.md                  (diese Datei)
├── firmware/
│   ├── esp32_controller/          (Wearable: liest IMUs, sendet ESP-NOW)
│   ├── esp32_receiver/            (Arm: empfaengt ESP-NOW, leitet an Arduino)
│   └── arduino_arm/               (Servo-Steuerung, Limits, Ramping)
├── hardware/
│   └── schaltplaene/              (Fritzing / KiCad Schaltplaene)
├── calibration/                   (Kalibrierungsdaten, Mapping-Tabellen)
└── docs/                          (Fotos, Messprotokolle, Notizen)
```
