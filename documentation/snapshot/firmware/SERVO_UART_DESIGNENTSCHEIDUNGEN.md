# Servo-UART Designentscheidungen

Dieses Dokument beschreibt alle Designentscheidungen rund um die UART-Kommunikation
zwischen ESP32 Receiver und Arduino Drive Board sowie die Servo-Steuerung.
Es dient als Referenz fuer beide Sketches:
- `firmware/arduino_arm/arduino_arm.ino`
- `firmware/esp32_receiver/test_sweep.h`
- `firmware/esp32_receiver/uart_frame.h`

---

## 1. Hardwareverbindung ESP32 → Arduino

### Pin-Wahl: ESP32 GPIO15 (TX) → Arduino D2 (RX via SoftwareSerial)

**Warum GPIO15 auf dem ESP32?**
- GPIO43/44 sind die Standard-UART-Pins des ESP32-S3, verursachten aber einen Boot-Loop
  (schnelles Gruen+Rot-Blinken). Ursache: Konflikt mit USB-Serial/JTAG bei `usb_mode=1`
  in der Custom-Board-Konfiguration (`robotic_arm_s3n16r8`).
- GPIO15 ist laut SCHEMATIC_CURRENT.md als UART TX zum Arduino vorgesehen.
- GPIO15 wurde physisch verifiziert: LED an GPIO15 blinkte korrekt.

**Warum D2 auf dem Arduino (SoftwareSerial)?**
- Das Adeept Drive Board hat keinen freien Hardware-UART-RX. D0 (Hardware RX) ist mit
  dem CH340G USB-to-Serial Chip verbunden und konnte ueber den WiFi-Header nicht
  angesprochen werden (alle Pins getestet, beide Upload-Switch-Stellungen).
- D2 ist ueber den **IO Port Header Pin 6** zugaenglich. Diese Zuordnung stammt aus
  dem offiziellen Board-Schaltplan (PDF extrahiert):
  Pin1=D13, Pin2=D12, Pin3=D8, Pin4=D7, Pin5=D4, **Pin6=D2**.
- SoftwareSerial auf D2 wurde erfolgreich getestet (rx_test.ino: D13-LED blinkt schnell
  bei Datenempfang).

**NICHT AENDERN:** GPIO15 und D2 sind die einzigen funktionierenden Pins fuer diese
Verbindung. Andere Kombinationen wurden ausfuehrlich getestet und funktionieren nicht.

### Baudrate: 9600

- SoftwareSerial auf dem ATmega328P ist bei hoeheren Baudraten unzuverlaessig.
- 9600 Baud reicht fuer 11-Byte-Frames: ~12ms pro Frame.
- Definiert in `uart_frame.h` als `UART_BAUD` und muss in beiden Sketches identisch sein.

### GND-Verbindung

- ESP32 GND und Arduino GND muessen verbunden sein (gemeinsame Masseleitung).
- Im Testaufbau ueber Breadboard-Rail realisiert.

---

## 2. SoftwareSerial + Servo: Das Jitter-Problem

### Kernproblem

Die Arduino Servo-Library nutzt **Timer1-Interrupts** fuer die PWM-Erzeugung (50Hz, 20ms
Periode). SoftwareSerial nutzt **Pin-Change-Interrupts (PCINT)** fuer den Byte-Empfang.
Beide Interrupt-Quellen konkurrieren um CPU-Zeit. Wenn ein PCINT waehrend eines
Timer1-ISR auftritt (oder umgekehrt), wird die PWM-Pulsbreite verfaelscht.

**Auswirkung:** Sichtbares Zucken/Zittern der Servos, besonders bei Gelenken unter Last
(Schulter). Das Zucken tritt auf, SOLANGE SoftwareSerial aktiv ist — nicht nur beim
Datenempfang, sondern allein durch die PCINT-Registrierung.

### Getestete Ansaetze (chronologisch)

1. **espSerial.end()/begin() um Servo-Writes** — SoftwareSerial nur beim Schreiben
   pausieren. **Fehlgeschlagen:** Jitter tritt zwischen den Writes auf, nicht waehrend.

2. **Zeitmultiplex (30ms listen / 70ms servo, Servos attached)** — SoftwareSerial und
   Servos wechseln sich ab, Servos bleiben attached. **Fehlgeschlagen:** Selbst 30ms
   SoftwareSerial verursacht sichtbares Zucken am gesamten Arm.

3. **Zeitmultiplex (15ms listen / 100ms servo, Servos attached)** — Kuerzeres
   SoftwareSerial-Fenster. **Fehlgeschlagen:** Immer noch Zucken.

4. **Detach mit 25ms PWM** — Servos nur 25ms attached, dann detach. Kein Jitter, aber
   Servos haben kein Haltemoment. **Fehlgeschlagen:** Greifer driftet ab, Gelenke
   rutschen unter Armgewicht.

5. **Detach mit 100ms PWM + 150ms Listen** — Aktueller Ansatz. Servos bekommen 100ms
   sauberes PWM-Signal (5 Zyklen bei 50Hz), dann detach und SoftwareSerial-Empfang.
   **FUNKTIONIERT:** Kein Jitter, ausreichend Haltemoment, Frames werden empfangen.

### Gewaehler Ansatz: Detach-Zyklus (100ms PWM / 100ms Listen)

```
[--- 100ms SoftwareSerial ---][--- 100ms Servo PWM (4×25ms Slew-Schritte) ---]
  Servos detached                Servos attached
  espSerial aktiv                Kein SoftwareSerial
  Kein PWM, kein Jitter          Sauberes PWM + Multi-Step Slew
  buf_pos Reset am Anfang!
```

**Zyklusdauer:** 200ms (5 Hz Update-Rate fuer Servos).

**Frame-Empfang:** Bei 50% Listen-Duty-Cycle und 20Hz Frame-Rate vom ESP32 werden
durchschnittlich 2 Frames pro Fenster empfangen. Der Slew-Rate-Limiter (Abschnitt 9)
glaettet die Bewegung auch bei gelegentlichem Frame-Verlust.

**Historisch:** Urspruenglich 100ms PWM / 150ms Listen (250ms Zyklus, 4 Hz). Die
Listen-Phase wurde auf 100ms reduziert als der Multi-Step-Slew (Abschnitt 10) eingefuehrt
wurde — die hoehere Update-Rate (5 Hz statt 4 Hz) kompensiert die kuerzere Empfangszeit.

**NICHT AENDERN:** Die 100ms/100ms Verhaeltnisse sind empirisch optimiert:
- Weniger als 100ms Servo-PWM: Servos erreichen Position nicht sicher, Drift unter Last.
  (60ms wurde getestet und verursachte Jitter/Zucken.)
- Weniger als 100ms Listen: Zu wenige Frames werden empfangen, ruckelige Bewegung.
- Servos waehrend Listen-Phase attached lassen: Sofortiges Jitter-Problem.

---

## 3. Servo attach()/write()-Reihenfolge

### Problem: Glitch-Puls bei attach()

Die Arduino Servo-Library sendet nach `attach()` sofort einen PWM-Puls mit dem
**Standardwert 1500us (= 90 Grad)**, bevor `write()` den korrekten Wert setzen kann.

Bei Gelenken weit weg von 90 Grad (z.B. Wrist bei 5 Grad oder 177 Grad) verursacht
dieser kurze 90-Grad-Puls einen sichtbaren Ruck.

### Loesung: Sofort nach attach() schreiben

```cpp
void servos_attach_und_schreiben() {
    servoBase.attach(SERVO_BASE_PIN);
    servoBase.write(ziel[0]);         // <-- sofort nach attach!
    servoShoulder.attach(SERVO_SHOULDER_PIN);
    servoShoulder.write(ziel[1]);     // <-- sofort nach attach!
    // ... usw.
}
```

Jeder Servo wird einzeln attached und SOFORT mit dem Zielwert beschrieben, bevor der
naechste Servo attached wird. So wird der 90-Grad-Glitch minimiert.

**NICHT AENDERN:** Die Reihenfolge attach→write pro Servo ist kritisch.
Nicht erst alle attachen und dann alle schreiben!

---

## 4. Servo-Limits (gemessene Werte)

Die MIN/MAX-Werte wurden manuell mit dem Potentiometer-Sketch (`potentio_debug.ino`)
gemessen. Jeder Servo wurde physisch an seine mechanischen Endanschlaege gefahren und
der Winkelwert ueber die serielle Konsole abgelesen.

| Gelenk   | Pin | Poti | MIN  | MAX  | Mittelwert | Beschreibung MIN               | Beschreibung MAX                |
|----------|-----|------|------|------|------------|--------------------------------|---------------------------------|
| Base     | D9  | A0   | 12   | 139  | 75         | Vollstaendig rechts gedreht    | Vollstaendig links (math. pos.) |
| Shoulder | D11 | A6   | 35   | 142  | 88         | Komplett eingeklappt           | Achtung: max=142 bewusst limitiert |
| Elbow    | D3  | A3   | 80   | 175  | 127        | Komplett gerade                | Komplett eingeklappt            |
| Wrist    | D5  | A2   | 5    | 177  | 91         | Im Uhrzeigersinn (math. neg.)  | Gegen Uhrzeigersinn (math. pos.) |
| Gripper  | D6  | A1   | 32   | 126  | 79         | Komplett geoeffnet             | Komplett geschlossen            |

**Shoulder-Limit (MAX=142):** Bewusst auf 142 begrenzt. Der Servo koennte physisch
weiter, aber der Nutzer hat bei der Messung explizit gesagt "nicht weiter drehen, das
wars, weiter soll der nicht duerfen". Grund: Mechanische Belastung und Kollisionsgefahr.

**Quelle:** `calibration/servo_limits/README.md`

**NICHT AENDERN:** Diese Werte sind hardware-spezifisch und gelten nur fuer diesen
konkreten Roboterarm. Bei Servo-Tausch oder mechanischer Aenderung neu messen!

### Pin-zu-Gelenk-Zuordnung

Die Zuordnung weicht vom Adeept-Standard ab und wurde empirisch ermittelt:
- D9 = Base (nicht Schulter wie im Original-Code)
- D11 = Shoulder
- D3 = Elbow
- D5 = Wrist
- D6 = Gripper

**Quelle:** `memory/project_servo_pin_mapping.md`

---

## 5. Startwerte und Neutralposition

### Warum Mittelwerte statt pauschal 90 Grad?

Urspruenglich starteten alle Servos bei `NEUTRAL = 90`. Das verursachte einen harten
Ruck beim ersten Frame-Empfang, weil die Mapping-Mittelwerte teils weit von 90 entfernt
sind:

| Gelenk   | Alter Start (90) | Neuer Start (Mitte) | Differenz |
|----------|-------------------|---------------------|-----------|
| Base     | 90                | 75                  | 15 Grad   |
| Shoulder | 90                | 88                  | 2 Grad    |
| Elbow    | 90                | 127                 | **37 Grad** |
| Wrist    | 90                | 91                  | 1 Grad    |
| Gripper  | 90                | 79                  | 11 Grad   |

Der Ellbogen sprang beim Start um 37 Grad — das war deutlich sichtbar und mechanisch
belastend. Die Mittelwerte `(MIN + MAX) / 2` entsprechen dem, was der ESP32-Sweep
bei `SWEEP_NEUTRAL = 128` sendet: `map(128, 0, 255, MIN, MAX) ≈ (MIN + MAX) / 2`.

**NICHT AENDERN:** Die Startwerte muessen mit dem ESP32-Sweep-Neutral uebereinstimmen.
Wenn SWEEP_NEUTRAL im ESP32 geaendert wird, muessen die Arduino-Startwerte angepasst
werden (oder umgekehrt).

---

## 6. Sweep-Generator (test_sweep.h)

### Zweck

Reiner Kettentest: ESP32 Receiver → UART → Arduino → Servos. Unabhaengig von ESP-NOW
und IMU-Daten. Erzeugt ein Pendel-Signal das die UART-Kette end-to-end verifiziert.

### Parameter

| Parameter        | Wert | Begruendung                                                |
|------------------|------|------------------------------------------------------------|
| SWEEP_NEUTRAL    | 128  | Mittelwert im 0-255 Byte-Raum. Mappt auf Servo-Mittelwerte. |
| SWEEP_AMPLITUDE  | 127  | Voller Bereich (1-255). Fuer Teilbereich reduzieren.       |
| SWEEP_INTERVALL  | 50   | 20 Hz Frame-Rate. Bei 60% Listen-Duty genug Frames.       |
| SWEEP_SCHRITT    | 2    | Inkrement pro Frame. Schnellere Bewegung bei hoeherem Wert. |

### int16_t statt int8_t fuer sweep_offset

**Bug-Historie:** `sweep_offset` war urspruenglich `int8_t` (Bereich -128 bis 127).
Bei Amplitude 127 und Schritt 2 erreichte der Offset 126+2=128, was zu einem
**Integer-Overflow auf -128** fuehrte. Der Servo-Wert sprang schlagartig von 254 auf 0
(voller Bereich). Beide betroffenen Achsen (Gripper + Wrist) zeigten den Fehler.

**Fix:** `int16_t` kann problemlos Werte bis +-32767 halten.

**NICHT AENDERN:** sweep_offset und sweep_richtung muessen int16_t bleiben. int8_t
fuehrt bei Amplitude > 63 (mit Schritt 2) oder > 127 (mit Schritt 1) zu Overflow.

### Achsen-Konfiguration

Aktuell bewegen sich ALLE 5 ACHSEN. Die Schulter hat eine reduzierte Amplitude (±80
statt ±127), um mechanische Belastung und Kollisionsgefahr zu minimieren:
```cpp
// Schulter nur minimal bewegen (±80 statt voller Amplitude)
int16_t schulter_offset = (sweep_offset * 80) / SWEEP_AMPLITUDE;
uint8_t schulter_wert = SWEEP_NEUTRAL + schulter_offset;

uart_frame_senden(FRAME_FLAG_VALID, wert, schulter_wert, wert, wert, wert);
//                                   Basis  Schulter     Ellbogen  Wrist  Gripper
```

Die Achsen wurden schrittweise hinzugefuegt und einzeln getestet:
1. Gripper allein — grundlegende UART-Kette verifiziert
2. Gripper + Handgelenk — zwei Achsen synchron
3. + Ellbogen — drei Achsen, Overflow-Bug entdeckt und gefixt (Abschnitt 12)
4. + Basis — vier Achsen, Frame-Alignment-Bug entdeckt und gefixt (Abschnitt 11)
5. + Schulter — alle fuenf, mit reduzierter Amplitude fuer Schulter

Reihenfolge der Argumente: Flags, Basis, Schulter, Ellbogen, Handgelenk, Greifer.

---

## 7. UART Frame V1 Protokoll

Vollstaendige Spezifikation: `firmware/UART_FRAME_V1.md`

### Kurzuebersicht

- 11 Bytes, packed struct
- Start: 0xA5, 0x5A (Magic Bytes zur Frame-Synchronisation)
- Version: 1
- Sequenznummer: Auto-Inkrement (Ueberlauf-sicher, uint8_t)
- Flags: VALID, NEUTRAL, DEGRADED, ESTOP
- 5 Achsen-Sollwerte: je 0-255 (werden auf Servo-Limits gemappt)
- CRC8: XOR ueber alle Bytes ausser CRC selbst

### Mapping 0-255 → Servo-Winkel

Der ESP32 sendet Werte im Bereich 0-255. Der Arduino mappt diese auf die realen
Servo-Limits:
```
map(byte_wert, 0, 255, SERVO_MIN, SERVO_MAX)
```

Beispiel Gripper: `map(128, 0, 255, 32, 126) = 79` (Mittelposition).

---

## 8. USB-Verhalten

### USB waehrend Betrieb

USB (Serial) am Arduino stoert den SoftwareSerial-Empfang. Die Hardware-UART-Interrupts
(TX auf D1) koennen die SoftwareSerial-PCINT-Timing verfaelschen. Insbesondere
`Serial.print()` waehrend aktiver SoftwareSerial-Phase fuehrt zu Frame-Verlust.

**Regel:** USB/Serial am Arduino nur zum Flashen verwenden. Im Betrieb USB abziehen.
Der Arduino wird ueber das Drive Board (Batterie/Netzteil) versorgt.

### Debug-Ausgaben

Wenn Debug-Ausgaben benoetigt werden:
1. `Serial.begin()` im Setup
2. `Serial.print()` NUR NACH `espSerial.end()` aufrufen (nie waehrend Listen-Phase)
3. Im Betrieb Debug wieder entfernen — jeder Serial.print() kostet Zeit im Zyklus

---

## 9. Slew-Rate-Limiter

### Problem: Harte Servo-Spruenge

Ohne Slew-Rate-Limiter springt der Servo sofort auf den neuen Zielwert. Bei Frame-Verlust
(ein Frame geht verloren, der naechste hat einen grossen Positions-Unterschied) fuehrt das
zu ruckartigen Bewegungen. Besonders sichtbar bei der Basis und Schulter.

### Loesung: aktuell[] interpoliert Richtung ziel[]

Zwei Arrays:
- `ziel[5]` — Sollwert aus dem letzten empfangenen Frame
- `aktuell[5]` — tatsaechliche Servo-Position, bewegt sich schrittweise Richtung ziel

Pro Sub-Step bewegt sich `aktuell[i]` maximal `MAX_SCHRITT` Grad Richtung `ziel[i]`:
```cpp
#define MAX_SCHRITT  2  // Grad pro Sub-Step

void slew_update() {
    for (int i = 0; i < 5; i++) {
        if (aktuell[i] < ziel[i])
            aktuell[i] = min(aktuell[i] + MAX_SCHRITT, ziel[i]);
        else if (aktuell[i] > ziel[i])
            aktuell[i] = max(aktuell[i] - MAX_SCHRITT, ziel[i]);
    }
}
```

### MAX_SCHRITT Tuning

Bei 4 Sub-Steps pro Zyklus (Abschnitt 10) und 5 Hz Zyklusrate:
- MAX_SCHRITT=2: max 4×2 = 8 Grad pro Zyklus = 40 Grad/Sekunde
- MAX_SCHRITT=3: max 4×3 = 12 Grad pro Zyklus = 60 Grad/Sekunde

Aktueller Wert (2) ist ein Kompromiss: schnell genug fuer fluessige Sweep-Bewegung,
langsam genug um Spruenge abzufangen.

---

## 10. Multi-Step Servo Phase

### Problem: Ruckelige Bewegung bei einzelnem Slew-Schritt

Mit nur einem `slew_update()` pro Zyklus bewegt sich der Servo max `MAX_SCHRITT` Grad
alle 200ms. Bei MAX_SCHRITT=2 sind das nur 10 Grad/Sekunde — zu langsam, die Bewegung
wirkt "poeppelpoepp" (stoepselnd) statt smooth.

### Loesung: 4 Sub-Steps innerhalb der Servo-Phase

Statt eines einzelnen attach→write→delay(100ms)→detach werden 4 kleinere Schritte
ausgefuehrt:
```cpp
// Phase 2: Multi-Step Servo
servoBase.attach(SERVO_BASE_PIN);
// ... alle attachen ...
for (int s = 0; s < 4; s++) {
    slew_update();             // +MAX_SCHRITT Grad pro Achse
    servoBase.write(aktuell[0]);
    // ... alle schreiben ...
    delay(25);                 // 4×25ms = 100ms Gesamt-PWM-Zeit
}
servos_detach();
```

4 Schritte × `MAX_SCHRITT`(2) = 8 Grad pro Zyklus. Bei 5 Hz = 40 Grad/Sekunde.
Die Servo-Position aendert sich in 25ms-Intervallen statt 200ms — deutlich fluessiger.

### Warum 4 Sub-Steps und nicht mehr?

- Gesamt-PWM-Zeit muss ~100ms bleiben (5 Zyklen bei 50Hz fuer stabile Position)
- Bei 4×25ms: jeder Sub-Step hat genau 1.25 PWM-Zyklen — knapp genug
- Mehr Sub-Steps (z.B. 10×10ms) waere zu kurz pro Step fuer zuverlaessiges PWM
- Weniger Sub-Steps (z.B. 2×50ms) waere wieder zu ruckelig

### NICHT AENDERN

Die Kombination 4 Sub-Steps × 25ms × MAX_SCHRITT=2 ist empirisch optimiert.
Aenderungen an einem Parameter erfordern Anpassung der anderen.

---

## 11. Frame-Alignment-Fix (buf_pos Reset)

### Bug

`buf_pos` (Position im 11-Byte Empfangspuffer) wurde zwischen Listen-Fenstern NICHT
zurueckgesetzt. Wenn ein Frame am Ende eines Listen-Fensters nur teilweise empfangen
wurde (z.B. 7 von 11 Bytes), startete das naechste Fenster bei `buf_pos=7`.

Die restlichen 4 Bytes des naechsten Fensters gehoerten aber zu einem NEUEN Frame,
nicht zum alten. Die zusammengesetzten 11 Bytes ergaben einen korrupten Frame.
Manchmal passte die CRC zufaellig — dann sprang ein Servo (besonders die Basis) auf
einen falschen Wert.

### Symptom

Die Basis sprang alle paar Durchgaenge einmal schlagartig in die entgegengesetzte
Richtung. Alle anderen Achsen liefen normal. Das Problem trat nur bei der Basis auf,
weil die Basis-Position im Frame am empfindlichsten auf Byte-Verschiebung reagiert
(Byte 5 im Frame — die vorderen Bytes sind Magic/Version/Sequenz/Flags).

### Fix

```cpp
void loop() {
    buf_pos = 0;  // KRITISCH: Am Anfang jedes Listen-Fensters zuruecksetzen!
    espSerial.begin(9600);
    // ...
}
```

Ein unvollstaendiger Frame wird verworfen. Das ist akzeptabel — der naechste vollstaendige
Frame kommt innerhalb weniger Millisekunden.

### NICHT AENDERN

`buf_pos = 0` MUSS vor `espSerial.begin()` stehen. Ohne diesen Reset treten sporadische
Servo-Spruenge auf.

---

## 12. uint8_t Overflow-Clamping im Sweep-Generator

### Bug (zusaetzlich zu Abschnitt 6 int16_t Fix)

Nachdem der int8_t→int16_t Fix (Abschnitt 6) angewandt wurde, konnte `sweep_offset`
den Wert `SWEEP_AMPLITUDE` (127) plus `SWEEP_SCHRITT` (2) = 129 erreichen, bevor die
Richtungsumkehr griff.

Die Berechnung `uint8_t wert = SWEEP_NEUTRAL + sweep_offset` = `128 + 129 = 257`
laeuft als uint8_t auf 1 ueber. Alle Servos sprangen schlagartig auf ihre Minimalposition.

### Unterschied zu Abschnitt 6

- Abschnitt 6: **int8_t Overflow** in `sweep_offset` selbst (126+2=128 → -128)
- Abschnitt 12: **uint8_t Overflow** in der `wert`-Berechnung (128+129=257 → 1)

Beide Bugs traten an der oberen Grenze des Sweeps auf, hatten aber unterschiedliche
Symptome: Abschnitt 6 verursachte einen Sprung auf Maximalposition, Abschnitt 12 auf
Minimalposition.

### Fix: Clamping statt einfacher Richtungsumkehr

```cpp
if (sweep_offset >= SWEEP_AMPLITUDE) {
    sweep_offset = SWEEP_AMPLITUDE;   // CLAMPEN! Sonst uint8_t Overflow
    sweep_richtung = -1;
}
if (sweep_offset <= -SWEEP_AMPLITUDE) {
    sweep_offset = -SWEEP_AMPLITUDE;  // 128 + (-127) = 1 ist OK
    sweep_richtung = 1;
}
```

Ohne Clamping: `sweep_offset` wird erst auf 129 gesetzt, DANN wird die Richtung umgekehrt.
Im naechsten `wert`-Berechnung ist der Wert bereits 257 (uint8_t: 1).

Mit Clamping: `sweep_offset` wird auf 127 begrenzt. `wert` = 128+127 = 255 (passt in uint8_t).

### NICHT AENDERN

Das Clamping ist essentiell. Die Bedingung muss `>=` sein (nicht `>`), damit der
Offset nie ueber die Amplitude hinausgeht.

---

## 13. I2C-Migration (Abloesung von SoftwareSerial)

### Motivation

SoftwareSerial war die Ursache saemtlicher Jitter-Probleme (Abschnitt 2). Der gesamte
Detach-Zyklus (Abschnitte 2, 3, 10, 11) war ein Workaround fuer die Inkompatibilitaet
von SoftwareSerial-PCINT und Servo-Timer1-Interrupts.

I2C (Wire-Library) nutzt die **TWI-Hardware-Peripherie** des ATmega328P. Der TWI-Interrupt
kollidiert NICHT mit Timer1 — Servos koennen dauerhaft attached bleiben.

### Hardware-Aenderung

```
Vorher (UART):
  ESP32 GPIO15 (TX) → Arduino D2 (SoftwareSerial RX)
  1 Datenleitung + GND

Nachher (I2C):
  ESP32 GPIO13 (SDA) → Arduino A4 (SDA) — OLED-Header Pin 3
  ESP32 GPIO14 (SCL) → Arduino A5 (SCL) — OLED-Header Pin 4
  GND                → GND               — bereits vorhanden
  2 Datenleitungen + GND
```

**OLED-Display:** Muss ausgesteckt werden — belegt den gleichen I2C-Bus (A4/A5) auf dem
Adeept Drive Board. Die OLED-Header-Pins werden stattdessen fuer die ESP32-Verbindung genutzt.

**Pull-Up-Widerstaende:** Die internen Pull-Ups des ATmega328P reichen fuer kurze
Breadboard-Distanzen bei 100kHz. Externe 4.7kΩ Pull-Ups optional bei Problemen.

**Spannungslevel:** ESP32-S3 ist 3.3V, ATmega328P laeuft auf 5V. I2C ist Open-Drain —
der Arduino erkennt 3.3V als HIGH (Schwelle ~2.5V bei 5V-Betrieb). Kein Level-Shifter noetig.

### I2C-Konfiguration

| Parameter | Wert | Begruendung |
|-----------|------|-------------|
| Slave-Adresse | 0x42 | Frei, kein Konflikt mit gaengigen Devices |
| Clock | 100kHz | Standard-Mode, zuverlaessig bei Breadboard |
| SDA (ESP32) | GPIO13 | Frei waehlbar (GPIO-Matrix) |
| SCL (ESP32) | GPIO14 | Frei waehlbar (GPIO-Matrix) |
| SDA (Arduino) | A4 | Fest (ATmega328P TWI Hardware) |
| SCL (Arduino) | A5 | Fest (ATmega328P TWI Hardware) |

### Architektur-Vereinfachung

```
Vorher (SoftwareSerial + Detach-Zyklus):
  loop():
    buf_pos = 0
    espSerial.begin(9600)
    [100ms Empfang, Servos detached]
    espSerial.end()
    [Servos attachen]
    [4x25ms Slew-Schritte]
    [Servos detachen]
    [Timeout pruefen]
  Zyklusdauer: 200ms (5 Hz)

Nachher (I2C + dauerhaft attached):
  ISR bei_i2c_empfang():
    Wire.read() → rx_buf[]    // NUR Bytes kopieren!
    rx_bereit = true
  loop():
    if (rx_bereit) frame_verarbeiten()  // map() + Validierung
    slew_update()
    servo.write()
    delay(20ms)
  Zyklusdauer: 20ms (50 Hz)
```

**10x hoehere Update-Rate** (50 Hz statt 5 Hz) bei drastisch einfacherem Code.
Kein Detach, kein buf_pos-Reset, kein Timing-Balanceakt.

### ISR-Minimal-Pattern (KRITISCH!)

**Bug:** Die erste I2C-Version setzte `map()` und `ziel[]`-Updates direkt im
`Wire.onReceive()`-Callback. `map()` auf einem 8-Bit AVR benoetigt 32-Bit
Multiplikation und Division — das dauert viele Taktzyklen. Wenn dieser ISR
waehrend eines Timer1-Servo-Pulses auftritt, wird die Pulsbreite verfaelscht.

**Symptom:** Alle Servos zuckten sporadisch, auch die auf Neutralposition standen.
Identisches Symptom wie beim SoftwareSerial-Jitter, nur seltener.

**Fix:** ISR macht NUR `Wire.read()` → `rx_buf[]` + Flag setzen. Die gesamte
Verarbeitung (Validierung, CRC, map(), ziel[]-Update) passiert in `loop()`.

```cpp
// ISR: Minimal! Nur Bytes kopieren.
void bei_i2c_empfang(int anzahl_bytes) {
    if (anzahl_bytes != FRAME_SIZE) { while (Wire.available()) Wire.read(); return; }
    for (uint8_t i = 0; i < FRAME_SIZE; i++) rx_buf[i] = Wire.read();
    rx_bereit = true;
}

// loop(): Verarbeitung mit Interrupt-sicherer Kopie
void frame_verarbeiten() {
    uint8_t buf[FRAME_SIZE];
    noInterrupts();
    memcpy(buf, rx_buf, FRAME_SIZE);  // Atomare Kopie
    rx_bereit = false;
    interrupts();
    // ... Validierung, map(), ziel[]-Update ...
}
```

**NICHT AENDERN:** Keine Berechnungen im ISR! Nur Wire.read() und Flag.

### Stromversorgung

Mit dauerhaft attached Servos ist der Strombedarf hoeher als beim Detach-Zyklus
(100% PWM statt 50%). **USB (500mA) reicht NICHT** — der Arm sackt unter Last ab
(besonders Schulter bei seitlich ausgestrecktem Arm).

**Pflicht:** Batterie-Versorgung (2x 18650 ueber Drive Board) im Betrieb.
USB nur zum Flashen.

### Sweep-Anpassungen fuer I2C

| Parameter | UART (alt) | I2C (neu) | Begruendung |
|-----------|-----------|-----------|-------------|
| Frame-Rate | 20 Hz | 50 Hz | Alle Frames kommen an, feinere Aufloesung |
| SWEEP_SCHRITT | 2 | 1 | Halber Schritt bei 2.5x Rate = gleiche Geschwindigkeit |
| MAX_SCHRITT | 2 | 1 | 50 Hz × 1 Grad = 50 Grad/s (ausreichend smooth) |
| ESP32 Loop-Delay | 100ms | 10ms | Muss <= SWEEP_INTERVALL sein |
| Schulter-Amplitude | ±80/127 | ±80/127 | Unveraendert — mechanische Begrenzung |
| Ellbogen-Amplitude | voll | ±60/127 | Volle Amplitude + synchroner Sweep = Schwung |

### Dateien

| Datei | Funktion |
|-------|----------|
| `firmware/esp32_receiver/i2c_frame.h` | I2C-Abstraktionsschicht (ESP32 Master) |
| `firmware/esp32_receiver/test_sweep_i2c.h` | Sweep-Generator fuer I2C-Kettentest |
| `firmware/esp32_receiver/esp32_receiver_i2c.ino` | Receiver-Sketch mit I2C statt UART |
| `firmware/arduino_arm/arduino_arm_i2c.ino` | Arduino-Sketch mit I2C-Slave + dauerhaft attached |

Die UART-Originaldateien (`uart_frame.h`, `test_sweep.h`, `arduino_arm.ino`,
`esp32_receiver.ino`) bleiben als Fallback erhalten.
