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

### Gewaehler Ansatz: Detach-Zyklus (100ms PWM / 150ms Listen)

```
[--- 100ms Servo PWM ---][--- 150ms SoftwareSerial ---]
  Servos attached            Servos detached
  Kein SoftwareSerial        espSerial aktiv
  Sauberes PWM               Kein PWM, kein Jitter
```

**Zyklusdauer:** 250ms (4 Hz Update-Rate fuer Servos).

**Frame-Empfang:** Bei 60% Listen-Duty-Cycle und 20Hz Frame-Rate vom ESP32 werden
genuegend Frames empfangen. Nicht jeder Frame kommt an, aber die Bewegung ist fluessig.

**NICHT AENDERN:** Die 100ms/150ms Verhaeltnisse sind empirisch optimiert:
- Weniger als 100ms Servo-PWM: Servos erreichen Position nicht sicher, Drift unter Last.
- Weniger als 150ms Listen: Zu wenige Frames werden empfangen, ruckelige Bewegung.
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

Aktuell bewegen sich nur Gripper und Handgelenk:
```cpp
uart_frame_senden(FRAME_FLAG_VALID, SWEEP_NEUTRAL, SWEEP_NEUTRAL, SWEEP_NEUTRAL, wert, wert);
//                                   Basis          Schulter       Ellbogen       Wrist  Gripper
```

Um weitere Achsen zu testen, den entsprechenden `SWEEP_NEUTRAL` durch `wert` ersetzen.
Reihenfolge der Argumente: Basis, Schulter, Ellbogen, Handgelenk, Greifer.

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
