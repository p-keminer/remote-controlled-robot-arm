// =============================================================================
// Arduino Arm Firmware — I2C Frame V1 Empfang + Servo-Steuerung
// =============================================================================
//
// Empfaengt I2cFrameV1-Frames vom ESP32 Receiver ueber I2C (Wire-Library).
// Arduino ist I2C-Slave (Adresse 0x42), ESP32 ist I2C-Master.
//
// ARCHITEKTUR: Dauerhaft attached (kein Detach-Zyklus!)
//   I2C nutzt die TWI-Hardware-Peripherie des ATmega328P. Der TWI-Interrupt
//   kollidiert NICHT mit Timer1 (Servo-PWM). Deshalb koennen Servos dauerhaft
//   attached bleiben — kein Jitter, kein Detach noetig.
//
//   Vergleich zur alten SoftwareSerial-Loesung:
//     SoftwareSerial: PCINT kollidiert mit Timer1 → Servo-Jitter
//                     → Detach-Zyklus noetig (100ms Listen / 100ms Servo)
//     I2C (Wire):     TWI-Interrupt kollidiert NICHT mit Timer1
//                     → Servos bleiben dauerhaft attached
//                     → Einfacher Code, smoothere Bewegung
//
//   I2C-Verbindung:
//     ESP32 GPIO13 (SDA) → Arduino A4 (SDA) — OLED-Header Pin 3
//     ESP32 GPIO14 (SCL) → Arduino A5 (SCL) — OLED-Header Pin 4
//     ESP32 GND          → Arduino GND       — gemeinsame Masse
//
//   Das OLED-Display muss dafuer ausgesteckt werden (belegt den gleichen Bus).
//
// ISR-DESIGN (KRITISCH!):
//   Der I2C-Empfangs-Callback (bei_i2c_empfang) laeuft im ISR-Kontext.
//   Dort wird NUR Wire.read() in rx_buf[] kopiert — keine Verarbeitung!
//   map()-Berechnungen im ISR verursachten Servo-Zucken (32-Bit Arithmetik
//   auf 8-Bit AVR dauert zu lang und stoert Timer1-PWM).
//   Validierung und map() passieren in loop() via frame_verarbeiten().
//   Siehe: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md Abschnitt 13
//
// NICHT AENDERN ohne das Designdokument zu lesen:
//   - Servo-Limits (firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md Abschnitt 4)
//   - Startwerte / Neutralposition (Abschnitt 5)
//   - Slew-Rate-Limiter (Abschnitt 9)
//   - I2C-Migration und ISR-Design (Abschnitt 13)
// =============================================================================

#include <Servo.h>
#include <Wire.h>

// =============================================================================
// I2C-Konfiguration
// =============================================================================
// Slave-Adresse: 0x42 — muss identisch mit I2C_SLAVE_ADDR in i2c_frame.h sein.
// 0x42 wurde gewaehlt weil keine gaengigen I2C-Devices diese Adresse nutzen.
#define I2C_ADDR  0x42

// =============================================================================
// Frame V1 — Protokolldefinition (identisch mit UART Frame V1)
// =============================================================================
// 11-Byte Binaerprotokoll. Gleiche Struktur wie beim UART-Transport.
// MUSS identisch sein mit firmware/esp32_receiver/i2c_frame.h

#define FRAME_START_A    0xA5   // Magic Byte 1 — Frame-Identifikation
#define FRAME_START_B    0x5A   // Magic Byte 2
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
} FrameV1;

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
// Timing
// =============================================================================
#define TIMEOUT_MS   1000   // Wenn 1s kein Frame: Neutral setzen
#define LOOP_MS      20     // 50 Hz Servo-Update-Rate (20ms = 1 PWM-Zyklus)

Servo servoBase, servoGripper, servoWrist, servoElbow, servoShoulder;

unsigned long letzter_frame_ms = 0;  // Zeitstempel des letzten gueltigen Frames

// =============================================================================
// Servo-Zielwinkel — Startwerte = Mittelposition jedes Gelenks
// =============================================================================
// WICHTIG: NICHT pauschal 90 Grad verwenden!
// Die Mittelwerte (MIN+MAX)/2 entsprechen dem, was der ESP32-Sweep bei
// SWEEP_NEUTRAL=128 sendet: map(128, 0, 255, MIN, MAX) ≈ (MIN+MAX)/2.
// Bei pauschal 90 Grad springt z.B. der Ellbogen um 37 Grad beim ersten Frame.
// Siehe: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md Abschnitt 5

// Maximale Grad-Aenderung pro Loop-Durchlauf (Slew-Rate-Limiter).
// Bei 50 Hz Loop und MAX_SCHRITT=1: max 50 Grad/Sekunde.
// Glaettet Bewegung und verhindert harte Spruenge.
#define MAX_SCHRITT  1

// ziel[] = Sollwert vom letzten empfangenen Frame (gesetzt im I2C-Interrupt)
// aktuell[] = tatsaechliche Servo-Position (interpoliert in loop())
// ziel[] wird NUR in loop() geschrieben (frame_verarbeiten()) und gelesen.
// Kein volatile noetig — ISR schreibt nur in rx_buf/rx_bereit.
int ziel[5] = {
    (BASE_MIN + BASE_MAX) / 2,         // 75 — Base Mitte
    (SHOULDER_MIN + SHOULDER_MAX) / 2,  // 88 — Shoulder Mitte
    (ELBOW_MIN + ELBOW_MAX) / 2,       // 127 — Elbow Mitte
    (WRIST_MIN + WRIST_MAX) / 2,       // 91 — Wrist Mitte
    (GRIPPER_MIN + GRIPPER_MAX) / 2    // 79 — Gripper Mitte
};
int aktuell[5] = {
    (BASE_MIN + BASE_MAX) / 2,
    (SHOULDER_MIN + SHOULDER_MAX) / 2,
    (ELBOW_MIN + ELBOW_MAX) / 2,
    (WRIST_MIN + WRIST_MAX) / 2,
    (GRIPPER_MIN + GRIPPER_MAX) / 2
};

// I2C-Empfangspuffer — ISR schreibt nur Rohbytes, loop() verarbeitet.
// GRUND: map() im ISR dauert zu lang auf 8-Bit AVR (32-Bit Multiplikation/Division)
// und stoert Timer1-Servo-PWM → sichtbares Zucken aller Servos.
volatile uint8_t rx_buf[FRAME_SIZE];
volatile bool rx_bereit = false;

// CRC8 — XOR ueber alle Bytes. Muss identisch mit i2c_frame.h sein.
uint8_t crc_berechnen(const uint8_t* daten, uint8_t laenge) {
    uint8_t summe = 0;
    for (uint8_t i = 0; i < laenge; i++) summe ^= daten[i];
    return summe;
}

// =============================================================================
// I2C Empfangs-Callback (ISR-Kontext!)
// =============================================================================
// NUR Bytes lesen und in rx_buf kopieren. Keinerlei Verarbeitung!
// Alles andere (Validierung, map(), ziel[]-Update) passiert in loop().
void bei_i2c_empfang(int anzahl_bytes) {
    if (anzahl_bytes != FRAME_SIZE) {
        while (Wire.available()) Wire.read();
        return;
    }
    for (uint8_t i = 0; i < FRAME_SIZE; i++) {
        rx_buf[i] = Wire.read();
    }
    rx_bereit = true;
}

// Frame aus rx_buf validieren und ziel[] aktualisieren.
// Wird in loop() aufgerufen — NICHT im ISR!
void frame_verarbeiten() {
    uint8_t buf[FRAME_SIZE];
    // Lokale Kopie erstellen (rx_buf ist volatile)
    noInterrupts();
    for (uint8_t i = 0; i < FRAME_SIZE; i++) buf[i] = rx_buf[i];
    rx_bereit = false;
    interrupts();

    // Magic Bytes pruefen
    if (buf[0] != FRAME_START_A || buf[1] != FRAME_START_B) return;

    FrameV1* frame = (FrameV1*)buf;

    if (frame->protokoll_version != FRAME_VERSION) return;
    if (frame->crc8 != crc_berechnen(buf, FRAME_SIZE - 1)) return;

    // Frame ist gueltig
    letzter_frame_ms = millis();

    if (frame->flags & FLAG_ESTOP || frame->flags & FLAG_NEUTRAL) {
        neutral_setzen();
        return;
    }
    if (!(frame->flags & FLAG_VALID)) return;

    ziel[0] = map(frame->basis_soll, 0, 255, BASE_MIN, BASE_MAX);
    ziel[1] = map(frame->schulter_soll, 0, 255, SHOULDER_MIN, SHOULDER_MAX);
    ziel[2] = map(frame->ellbogen_soll, 0, 255, ELBOW_MIN, ELBOW_MAX);
    ziel[3] = map(frame->handgelenk_soll, 0, 255, WRIST_MIN, WRIST_MAX);
    ziel[4] = map(frame->greifer_soll, 0, 255, GRIPPER_MIN, GRIPPER_MAX);
}

// =============================================================================
// Servo-Steuerung
// =============================================================================

// Slew-Rate-Limiter: aktuell[] bewegt sich pro Durchlauf max MAX_SCHRITT Grad
// Richtung ziel[]. Glaettet Bewegung und verhindert harte Spruenge.
void slew_update() {
    for (int i = 0; i < 5; i++) {
        if (aktuell[i] < ziel[i]) {
            aktuell[i] = min(aktuell[i] + MAX_SCHRITT, ziel[i]);
        } else if (aktuell[i] > ziel[i]) {
            aktuell[i] = max(aktuell[i] - MAX_SCHRITT, ziel[i]);
        }
    }
}

// Alle Achsen auf Mittelposition setzen (bei Timeout).
// Verwendet (MIN+MAX)/2 statt pauschal 90 — siehe Abschnitt 5 im Designdokument.
void neutral_setzen() {
    ziel[0] = (BASE_MIN + BASE_MAX) / 2;
    ziel[1] = (SHOULDER_MIN + SHOULDER_MAX) / 2;
    ziel[2] = (ELBOW_MIN + ELBOW_MAX) / 2;
    ziel[3] = (WRIST_MIN + WRIST_MAX) / 2;
    ziel[4] = (GRIPPER_MIN + GRIPPER_MAX) / 2;
}

// =============================================================================
// Setup
// =============================================================================
void setup() {
    // Servos einmalig attachen und auf Startposition fahren.
    // REIHENFOLGE KRITISCH: attach() sendet kurz 90-Grad-Puls (Servo-Library Default).
    // Durch sofortiges write() nach jedem attach() wird der Glitch minimiert.
    // Siehe: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md Abschnitt 3
    servoBase.attach(SERVO_BASE_PIN);
    servoBase.write(aktuell[0]);
    servoShoulder.attach(SERVO_SHOULDER_PIN);
    servoShoulder.write(aktuell[1]);
    servoElbow.attach(SERVO_ELBOW_PIN);
    servoElbow.write(aktuell[2]);
    servoWrist.attach(SERVO_WRIST_PIN);
    servoWrist.write(aktuell[3]);
    servoGripper.attach(SERVO_GRIPPER_PIN);
    servoGripper.write(aktuell[4]);

    // I2C-Slave initialisieren
    Wire.begin(I2C_ADDR);
    Wire.onReceive(bei_i2c_empfang);

    letzter_frame_ms = millis();
}

// =============================================================================
// Hauptschleife — einfacher 50 Hz Servo-Update
// =============================================================================
// Kein Detach-Zyklus mehr! I2C-Empfang laeuft im Hintergrund per TWI-Interrupt.
// loop() macht nur noch:
//   1. Slew-Update (aktuell[] Richtung ziel[] bewegen)
//   2. Servo-Positionen schreiben
//   3. Timeout pruefen
//   4. 20ms warten (= 50 Hz, passt zu Servo-PWM-Frequenz)
void loop() {
    // Neuen Frame verarbeiten (Validierung + map() hier statt im ISR)
    if (rx_bereit) {
        frame_verarbeiten();
    }

    // Slew-Update und Servo-Positionen schreiben
    slew_update();
    servoBase.write(aktuell[0]);
    servoShoulder.write(aktuell[1]);
    servoElbow.write(aktuell[2]);
    servoWrist.write(aktuell[3]);
    servoGripper.write(aktuell[4]);

    // Timeout: Keine Frames seit 1 Sekunde → Sicherheits-Neutral
    if (millis() - letzter_frame_ms > TIMEOUT_MS) {
        neutral_setzen();
        letzter_frame_ms = millis();
    }

    delay(LOOP_MS);
}
