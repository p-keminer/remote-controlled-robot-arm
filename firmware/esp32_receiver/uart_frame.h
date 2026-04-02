#ifndef UART_FRAME_H
#define UART_FRAME_H

// =============================================================================
// UART Frame V1 — Abstraktionsschicht fuer ESP32 Receiver → Arduino
// =============================================================================
//
// Sendet 11-Byte Binaer-Frames ueber Serial1 (Hardware-UART) an den Arduino.
// Der Arduino empfaengt diese via SoftwareSerial auf D2.
//
// Vollstaendige Protokoll-Spezifikation: firmware/UART_FRAME_V1.md
// Hardware/Timing-Entscheidungen: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md
//
// FRAME-STRUKTUR (11 Bytes):
//   [0xA5][0x5A][Version][Sequenz][Flags][Basis][Schulter][Ellbogen][Wrist][Gripper][CRC8]
//
// Die Struct-Definition hier MUSS identisch sein mit der in arduino_arm.ino!
// =============================================================================

#include <Arduino.h>

#define UART_FRAME_START_A   0xA5  // Magic Byte 1 — Frame-Synchronisation
#define UART_FRAME_START_B   0x5A  // Magic Byte 2

#define UART_FRAME_VERSION   1     // Protokollversion — Arduino ignoriert andere Versionen

// UART1 TX Pin: GPIO15
// WARUM GPIO15: GPIO43/44 (Standard-UART) verursachen Boot-Loop bei usb_mode=1
// auf dem Custom ESP32-S3 Board (robotic_arm_s3n16r8). GPIO15 ist laut
// SCHEMATIC_CURRENT.md als UART TX zum Arduino vorgesehen und funktioniert.
// Siehe: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md Abschnitt 1
// NICHT AENDERN ohne Hardware-Test!
#define UART_TX_PIN          15

// Baudrate: 9600 — SoftwareSerial auf ATmega328P ist bei hoeheren Raten unzuverlaessig.
// MUSS identisch sein mit espSerial.begin() im Arduino-Sketch!
#define UART_BAUD            9600

// Flags — bestimmen wie der Arduino die Achsenwerte interpretiert
#define FRAME_FLAG_VALID     (1 << 0)  // Daten gueltig, Achsenwerte anwenden
#define FRAME_FLAG_NEUTRAL   (1 << 1)  // Alle Achsen auf Neutralposition
#define FRAME_FLAG_DEGRADED  (1 << 2)  // Sensordaten teilweise unzuverlaessig (Info)
#define FRAME_FLAG_ESTOP     (1 << 3)  // Notaus — sofort auf Neutral

typedef struct __attribute__((packed)) {
    uint8_t start_a;            // 0xA5
    uint8_t start_b;            // 0x5A
    uint8_t protokoll_version;  // UART_FRAME_VERSION
    uint8_t sequenz;            // Auto-Inkrement (uint8_t, Ueberlauf OK)
    uint8_t flags;              // Kombination der FRAME_FLAG_* Bits
    uint8_t basis_soll;         // 0-255, Arduino mappt auf BASE_MIN..BASE_MAX
    uint8_t schulter_soll;      // 0-255, Arduino mappt auf SHOULDER_MIN..SHOULDER_MAX
    uint8_t ellbogen_soll;      // 0-255, Arduino mappt auf ELBOW_MIN..ELBOW_MAX
    uint8_t handgelenk_soll;    // 0-255, Arduino mappt auf WRIST_MIN..WRIST_MAX
    uint8_t greifer_soll;       // 0-255, Arduino mappt auf GRIPPER_MIN..GRIPPER_MAX
    uint8_t crc8;               // XOR ueber Bytes 0-9 (alles ausser CRC)
} UartFrameV1;

// Sequenznummer — wird bei jedem gesendeten Frame automatisch erhoeht.
// uint8_t: Ueberlauf von 255 auf 0 ist gewollt und unproblematisch.
static uint8_t uart_sequenz = 0;

// CRC8 berechnen: XOR ueber alle Bytes der Struct ausser dem letzten (CRC selbst).
// Identische Implementierung muss im Arduino-Sketch existieren!
static uint8_t uart_crc_berechnen(const UartFrameV1* frame) {
    const uint8_t* bytes = (const uint8_t*)frame;
    uint8_t summe = 0;
    for (size_t i = 0; i < sizeof(UartFrameV1) - 1; i++) {
        summe ^= bytes[i];
    }
    return summe;
}

// UART initialisieren — nur TX, kein RX (Einweg-Kommunikation zum Arduino).
// Wird einmal in setup() aufgerufen.
static void uart_frame_init() {
    Serial1.begin(UART_BAUD, SERIAL_8N1, -1, UART_TX_PIN);  // RX=-1 (nicht benutzt)
}

// Frame zusammenbauen und senden.
// Parameter sind Roh-Byte-Werte (0-255) — das Mapping auf Servo-Winkel
// macht der Arduino (map(wert, 0, 255, MIN, MAX)).
// Argumente-Reihenfolge: basis, schulter, ellbogen, handgelenk, greifer
static void uart_frame_senden(uint8_t flags, uint8_t basis, uint8_t schulter,
                               uint8_t ellbogen, uint8_t handgelenk, uint8_t greifer) {
    UartFrameV1 frame;
    frame.start_a = UART_FRAME_START_A;
    frame.start_b = UART_FRAME_START_B;
    frame.protokoll_version = UART_FRAME_VERSION;
    frame.sequenz = uart_sequenz++;
    frame.flags = flags;
    frame.basis_soll = basis;
    frame.schulter_soll = schulter;
    frame.ellbogen_soll = ellbogen;
    frame.handgelenk_soll = handgelenk;
    frame.greifer_soll = greifer;
    frame.crc8 = uart_crc_berechnen(&frame);

    Serial1.write((const uint8_t*)&frame, sizeof(frame));
}

#endif
