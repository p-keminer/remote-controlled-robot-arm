// =============================================================================
// Arduino Arm Firmware — UART Frame V1 Empfang + Servo-Steuerung
// =============================================================================
//
// Empfaengt UartFrameV1-Frames vom ESP32 Receiver ueber SoftwareSerial auf D2
// (IO Port Header Pin 6) und steuert 5 Servos des Adeept Robotic Arm.
//
// ARCHITEKTUR: Detach-Zyklus (Zeitmultiplex)
//   SoftwareSerial (PCINT) und Servo-PWM (Timer1) koennen auf dem ATmega328P
//   nicht gleichzeitig laufen, ohne dass die Servos zucken (Jitter).
//   Loesung: Servos und SoftwareSerial wechseln sich in festen Zeitfenstern ab.
//
//   [--- 150ms Listen ---][--- 100ms Servo PWM ---]
//     SoftwareSerial aktiv    Servos attached + PWM
//     Servos detached         SoftwareSerial aus
//
//   Siehe: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md (Abschnitte 2-3)
//
// NICHT AENDERN ohne das Designdokument zu lesen:
//   - Pin-Zuordnungen (Abschnitt 1 + 4)
//   - Servo-Limits (Abschnitt 4)
//   - Timing SERVO_MS / LISTEN_MS (Abschnitt 2)
//   - attach/write-Reihenfolge (Abschnitt 3)
//   - Startwerte / Neutralposition (Abschnitt 5)
// =============================================================================

#include <Servo.h>
#include <SoftwareSerial.h>

// RX auf D2 = IO Port Header Pin 6. TX unbenutzt (-1).
// WARUM D2: Hardware-RX (D0) ist durch CH340G USB-Chip belegt und ueber den
// WiFi-Header nicht erreichbar. Alle anderen Pins wurden getestet — nur D2 funktioniert.
// Siehe: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md Abschnitt 1
SoftwareSerial espSerial(2, -1);

// =============================================================================
// UART Frame V1 — Protokolldefinition
// =============================================================================
// 11-Byte Binaerprotokoll. Vollstaendige Spec: firmware/UART_FRAME_V1.md
// MUSS identisch sein mit firmware/esp32_receiver/uart_frame.h

#define FRAME_START_A    0xA5   // Magic Byte 1 — Frame-Synchronisation
#define FRAME_START_B    0x5A   // Magic Byte 2 — zusammen eindeutiges Startmuster
#define FRAME_VERSION    1      // Protokollversion — Frames anderer Version werden ignoriert
#define FRAME_SIZE       11     // Gesamtgroesse in Bytes inkl. CRC

#define FLAG_VALID       (1 << 0)  // Bit 0: Daten sind gueltig und sollen angewandt werden
#define FLAG_NEUTRAL     (1 << 1)  // Bit 1: Alle Achsen auf Neutralposition fahren
#define FLAG_ESTOP       (1 << 3)  // Bit 3: Notaus — sofort auf Neutral

typedef struct __attribute__((packed)) {
    uint8_t start_a;            // 0xA5
    uint8_t start_b;            // 0x5A
    uint8_t protokoll_version;  // Muss FRAME_VERSION entsprechen
    uint8_t sequenz;            // Auto-Inkrement, fuer Debugging/Paketverlust-Erkennung
    uint8_t flags;              // FLAG_VALID, FLAG_NEUTRAL, FLAG_ESTOP
    uint8_t basis_soll;         // 0-255, wird auf BASE_MIN..BASE_MAX gemappt
    uint8_t schulter_soll;      // 0-255, wird auf SHOULDER_MIN..SHOULDER_MAX gemappt
    uint8_t ellbogen_soll;      // 0-255, wird auf ELBOW_MIN..ELBOW_MAX gemappt
    uint8_t handgelenk_soll;    // 0-255, wird auf WRIST_MIN..WRIST_MAX gemappt
    uint8_t greifer_soll;       // 0-255, wird auf GRIPPER_MIN..GRIPPER_MAX gemappt
    uint8_t crc8;               // XOR ueber Bytes 0-9
} UartFrameV1;

// =============================================================================
// Servo-Konfiguration — reale Verkabelung + gemessene Limits
// =============================================================================
// Pin-Zuordnung weicht vom Adeept-Standard ab! Empirisch ermittelt.
// Quelle: memory/project_servo_pin_mapping.md
// Limits: Mit Potentiometer-Sketch an physischen Endanschlaegen gemessen.
// Quelle: calibration/servo_limits/README.md
// Details: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md Abschnitt 4
//
// NICHT AENDERN ohne physische Nachmessung am Arm!

#define SERVO_BASE_PIN      9    // Base/Drehung — Poti A0
#define SERVO_GRIPPER_PIN   6    // Greifer — Poti A1
#define SERVO_WRIST_PIN     5    // Handgelenk — Poti A2
#define SERVO_ELBOW_PIN     3    // Ellbogen — Poti A3
#define SERVO_SHOULDER_PIN  11   // Schulter — Poti A6

#define BASE_MIN       12   // Vollstaendig rechts gedreht
#define BASE_MAX      139   // Vollstaendig links (mathematisch positiv)
#define GRIPPER_MIN    32   // Komplett geoeffnet
#define GRIPPER_MAX   126   // Komplett geschlossen
#define WRIST_MIN       5   // Im Uhrzeigersinn (mathematisch negativ)
#define WRIST_MAX     177   // Gegen Uhrzeigersinn (mathematisch positiv)
#define ELBOW_MIN      80   // Komplett gerade (Arm gestreckt)
#define ELBOW_MAX     175   // Komplett eingeklappt
#define SHOULDER_MIN   35   // Komplett eingeklappt
#define SHOULDER_MAX  142   // BEWUSST LIMITIERT — physisch gaenge mehr, aber
                            // mechanische Belastung und Kollisionsgefahr!

// =============================================================================
// Timing-Konfiguration — Detach-Zyklus
// =============================================================================
// NICHT AENDERN ohne Testlauf am echten Arm!
// Siehe: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md Abschnitt 2

#define TIMEOUT_MS   1000   // Wenn 1s kein Frame: Neutral setzen
#define SERVO_MS     100    // Servos attached mit sauberem PWM (5 Zyklen bei 50Hz)
                            // Weniger = Servos erreichen Position nicht sicher
#define LISTEN_MS    150    // SoftwareSerial aktiv, Servos detached
                            // Weniger = zu wenige Frames empfangen (ruckelig)
                            // Ein Frame @ 9600 Baud = ~12ms (11 Bytes)

Servo servoBase, servoGripper, servoWrist, servoElbow, servoShoulder;

uint8_t buf[FRAME_SIZE];        // Empfangspuffer fuer einen Frame
uint8_t buf_pos = 0;            // Aktuelle Position im Empfangspuffer
unsigned long letzter_frame_ms = 0;  // Zeitstempel des letzten gueltigen Frames

// =============================================================================
// Servo-Zielwinkel — Startwerte = Mittelposition jedes Gelenks
// =============================================================================
// WICHTIG: NICHT pauschal 90 Grad verwenden!
// Die Mittelwerte (MIN+MAX)/2 entsprechen dem, was der ESP32-Sweep bei
// SWEEP_NEUTRAL=128 sendet: map(128, 0, 255, MIN, MAX) ≈ (MIN+MAX)/2.
// Bei pauschal 90 Grad springt z.B. der Ellbogen um 37 Grad beim ersten Frame.
// Siehe: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md Abschnitt 5

int ziel[5] = {
    (BASE_MIN + BASE_MAX) / 2,         // 75 — Base Mitte
    (SHOULDER_MIN + SHOULDER_MAX) / 2,  // 88 — Shoulder Mitte
    (ELBOW_MIN + ELBOW_MAX) / 2,       // 127 — Elbow Mitte
    (WRIST_MIN + WRIST_MAX) / 2,       // 91 — Wrist Mitte
    (GRIPPER_MIN + GRIPPER_MAX) / 2    // 79 — Gripper Mitte
};

// CRC8 — XOR ueber alle Bytes. Muss identisch mit uart_frame.h sein.
uint8_t crc_berechnen(const uint8_t* daten, uint8_t laenge) {
    uint8_t summe = 0;
    for (uint8_t i = 0; i < laenge; i++) summe ^= daten[i];
    return summe;
}

// =============================================================================
// Servo-Steuerung
// =============================================================================

// Servos einzeln attachen und SOFORT danach schreiben.
// REIHENFOLGE KRITISCH: attach() sendet kurz 90-Grad-Puls (Servo-Library Default).
// Bei Gelenken weit von 90 (z.B. Wrist bei 5 oder 177) verursacht das einen Ruck.
// Durch sofortiges write() nach jedem attach() wird der Glitch minimiert.
// Siehe: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md Abschnitt 3
// NICHT AENDERN: Nicht erst alle attachen und dann alle schreiben!
void servos_attach_und_schreiben() {
    servoBase.attach(SERVO_BASE_PIN);
    servoBase.write(ziel[0]);
    servoShoulder.attach(SERVO_SHOULDER_PIN);
    servoShoulder.write(ziel[1]);
    servoElbow.attach(SERVO_ELBOW_PIN);
    servoElbow.write(ziel[2]);
    servoWrist.attach(SERVO_WRIST_PIN);
    servoWrist.write(ziel[3]);
    servoGripper.attach(SERVO_GRIPPER_PIN);
    servoGripper.write(ziel[4]);
}

// Alle Servos detachen — kein PWM-Signal mehr, kein Haltemoment.
// Wird benutzt damit SoftwareSerial-Interrupts keine PWM-Stoerungen verursachen.
void servos_detach() {
    servoBase.detach();
    servoShoulder.detach();
    servoElbow.detach();
    servoWrist.detach();
    servoGripper.detach();
}

// Alle Achsen auf Mittelposition setzen (bei Timeout oder ESTOP/NEUTRAL-Flag).
// Verwendet (MIN+MAX)/2 statt pauschal 90 — siehe Abschnitt 5 im Designdokument.
void neutral_setzen() {
    ziel[0] = (BASE_MIN + BASE_MAX) / 2;
    ziel[1] = (SHOULDER_MIN + SHOULDER_MAX) / 2;
    ziel[2] = (ELBOW_MIN + ELBOW_MAX) / 2;
    ziel[3] = (WRIST_MIN + WRIST_MAX) / 2;
    ziel[4] = (GRIPPER_MIN + GRIPPER_MAX) / 2;
}

// Empfangenen Frame auf Servo-Zielwinkel umrechnen.
// Mapping: 0-255 Byte-Wert → MIN..MAX Grad pro Achse.
// Flags werden geprueft: ESTOP/NEUTRAL → Neutralposition, !VALID → ignorieren.
void frame_verarbeiten(const UartFrameV1* f) {
    if (f->flags & FLAG_ESTOP || f->flags & FLAG_NEUTRAL) {
        neutral_setzen();
        return;
    }
    if (!(f->flags & FLAG_VALID)) return;

    ziel[0] = map(f->basis_soll, 0, 255, BASE_MIN, BASE_MAX);
    ziel[1] = map(f->schulter_soll, 0, 255, SHOULDER_MIN, SHOULDER_MAX);
    ziel[2] = map(f->ellbogen_soll, 0, 255, ELBOW_MIN, ELBOW_MAX);
    ziel[3] = map(f->handgelenk_soll, 0, 255, WRIST_MIN, WRIST_MAX);
    ziel[4] = map(f->greifer_soll, 0, 255, GRIPPER_MIN, GRIPPER_MAX);
}

// =============================================================================
// Setup
// =============================================================================
void setup() {
    // Servos einmalig auf Startposition fahren und dann detachen.
    // Der Detach-Zyklus in loop() uebernimmt ab hier.
    servos_attach_und_schreiben();
    delay(SERVO_MS);
    servos_detach();
    letzter_frame_ms = millis();
}

// =============================================================================
// Hauptschleife — Detach-Zyklus
// =============================================================================
// Ablauf pro Iteration (~250ms):
//   1. SoftwareSerial einschalten, 150ms lang Frames empfangen (Servos detached)
//   2. SoftwareSerial ausschalten
//   3. Servos attachen, Zielwinkel schreiben, 100ms PWM halten
//   4. Servos detachen
//   5. Timeout pruefen
//
// WARUM diese Reihenfolge:
//   - SoftwareSerial und Servo-PWM duerfen NIEMALS gleichzeitig aktiv sein
//   - Servos brauchen mindestens ~100ms (5 PWM-Zyklen) fuer stabile Position
//   - 150ms Listen bei 20Hz Frame-Rate = durchschnittlich 3 Frames pro Fenster
void loop() {
    // --- Phase 1: UART-Daten lesen (Servos sind detached, kein PWM-Jitter) ---
    espSerial.begin(9600);
    unsigned long phase_start = millis();
    while (millis() - phase_start < LISTEN_MS) {
        while (espSerial.available()) {
            uint8_t b = espSerial.read();

            // Frame-Synchronisation: Erstes Byte muss 0xA5 sein
            if (buf_pos == 0 && b != FRAME_START_A) continue;
            // Zweites Byte muss 0x5A sein, sonst Reset
            if (buf_pos == 1 && b != FRAME_START_B) { buf_pos = 0; continue; }

            buf[buf_pos++] = b;

            // Vollstaendiger Frame empfangen?
            if (buf_pos == FRAME_SIZE) {
                buf_pos = 0;
                UartFrameV1* frame = (UartFrameV1*)buf;

                // Validierung: Version und CRC muessen stimmen
                if (frame->protokoll_version != FRAME_VERSION) continue;
                if (frame->crc8 != crc_berechnen(buf, FRAME_SIZE - 1)) continue;

                // Frame ist gueltig — Zielwinkel aktualisieren
                letzter_frame_ms = millis();
                frame_verarbeiten(frame);
            }
        }
    }
    espSerial.end();  // SoftwareSerial AUS — ab hier saubere Timer-Interrupts

    // --- Phase 2: Servos mit sauberem PWM versorgen ---
    servos_attach_und_schreiben();
    delay(SERVO_MS);
    servos_detach();

    // --- Timeout: Keine Frames seit 1 Sekunde → Sicherheits-Neutral ---
    if (millis() - letzter_frame_ms > TIMEOUT_MS) {
        neutral_setzen();
        letzter_frame_ms = millis();  // Verhindert Neutral-Spam bei anhaltendem Timeout
    }
}
