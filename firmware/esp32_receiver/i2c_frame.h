#ifndef I2C_FRAME_H
#define I2C_FRAME_H

// =============================================================================
// I2C Frame V1 — Abstraktionsschicht fuer ESP32 Receiver → Arduino
// =============================================================================
//
// Sendet 11-Byte Binaer-Frames ueber I2C (Wire) an den Arduino.
// ESP32 ist I2C-Master, Arduino ist I2C-Slave (Adresse I2C_SLAVE_ADDR).
//
// VORTEIL gegenueber UART (SoftwareSerial):
//   I2C nutzt die TWI-Hardware-Peripherie des ATmega328P. Der TWI-Interrupt
//   kollidiert NICHT mit Timer1 (Servo-PWM). Servos koennen dauerhaft attached
//   bleiben — kein Detach-Zyklus, kein Jitter.
//   Siehe: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md
//
// HARDWARE:
//   ESP32 GPIO13 (SDA) → Arduino A4 (SDA) — ueber OLED-Header Pin 3
//   ESP32 GPIO14 (SCL) → Arduino A5 (SCL) — ueber OLED-Header Pin 4
//   ESP32 GND          → Arduino GND       — gemeinsame Masse (bereits vorhanden)
//
// FRAME-STRUKTUR (11 Bytes, identisch mit UART Frame V1):
//   [0xA5][0x5A][Version][Sequenz][Flags][Basis][Schulter][Ellbogen][Wrist][Gripper][CRC8]
//
// Die Frame-Struktur ist absichtlich identisch mit dem UART-Protokoll.
// So kann spaeter zwischen I2C und UART gewechselt werden ohne den Arduino
// Frame-Parser aendern zu muessen.
//
// Die Struct-Definition hier MUSS identisch sein mit der in arduino_arm.ino!
// =============================================================================

#include <Arduino.h>
#include <Wire.h>

// I2C Slave-Adresse des Arduino. Muss in beiden Sketches identisch sein.
// 0x42 ist frei (kein gaengiges I2C-Device nutzt diese Adresse).
#define I2C_SLAVE_ADDR   0x42

// I2C Pins auf dem ESP32-S3 (frei waehlbar dank GPIO-Matrix)
#define I2C_SDA_PIN      13
#define I2C_SCL_PIN      14

// I2C Clock: 100kHz (Standard-Mode). Der ATmega328P unterstuetzt bis 400kHz,
// aber 100kHz ist zuverlaessiger bei Breadboard-Verbindungen.
#define I2C_CLOCK        100000

#define I2C_FRAME_START_A   0xA5  // Magic Byte 1 — Frame-Identifikation
#define I2C_FRAME_START_B   0x5A  // Magic Byte 2

#define I2C_FRAME_VERSION   1     // Protokollversion — Arduino ignoriert andere Versionen

// Flags — bestimmen wie der Arduino die Achsenwerte interpretiert
#define FRAME_FLAG_VALID     (1 << 0)  // Daten gueltig, Achsenwerte anwenden
#define FRAME_FLAG_NEUTRAL   (1 << 1)  // Alle Achsen auf Neutralposition
#define FRAME_FLAG_DEGRADED  (1 << 2)  // Sensordaten teilweise unzuverlaessig (Info)
#define FRAME_FLAG_ESTOP     (1 << 3)  // Notaus — sofort auf Neutral

typedef struct __attribute__((packed)) {
    uint8_t start_a;            // 0xA5
    uint8_t start_b;            // 0x5A
    uint8_t protokoll_version;  // I2C_FRAME_VERSION
    uint8_t sequenz;            // Auto-Inkrement (uint8_t, Ueberlauf OK)
    uint8_t flags;              // Kombination der FRAME_FLAG_* Bits
    uint8_t basis_soll;         // 0-255, Arduino mappt auf BASE_MIN..BASE_MAX
    uint8_t schulter_soll;      // 0-255, Arduino mappt auf SHOULDER_MIN..SHOULDER_MAX
    uint8_t ellbogen_soll;      // 0-255, Arduino mappt auf ELBOW_MIN..ELBOW_MAX
    uint8_t handgelenk_soll;    // 0-255, Arduino mappt auf WRIST_MIN..WRIST_MAX
    uint8_t greifer_soll;       // 0-255, Arduino mappt auf GRIPPER_MIN..GRIPPER_MAX
    uint8_t crc8;               // XOR ueber Bytes 0-9 (alles ausser CRC)
} I2cFrameV1;

// Sequenznummer — wird bei jedem gesendeten Frame automatisch erhoeht.
// uint8_t: Ueberlauf von 255 auf 0 ist gewollt und unproblematisch.
static uint8_t i2c_sequenz = 0;

// CRC8 berechnen: XOR ueber alle Bytes der Struct ausser dem letzten (CRC selbst).
// Identische Implementierung muss im Arduino-Sketch existieren!
static uint8_t i2c_crc_berechnen(const I2cFrameV1* frame) {
    const uint8_t* bytes = (const uint8_t*)frame;
    uint8_t summe = 0;
    for (size_t i = 0; i < sizeof(I2cFrameV1) - 1; i++) {
        summe ^= bytes[i];
    }
    return summe;
}

// I2C initialisieren — ESP32 als Master.
// Wird einmal in setup() aufgerufen.
static void i2c_frame_init() {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(I2C_CLOCK);
}

// Frame zusammenbauen und per I2C an den Arduino-Slave senden.
// Parameter sind Roh-Byte-Werte (0-255) — das Mapping auf Servo-Winkel
// macht der Arduino (map(wert, 0, 255, MIN, MAX)).
// Argumente-Reihenfolge: flags, basis, schulter, ellbogen, handgelenk, greifer
static void i2c_frame_senden(uint8_t flags, uint8_t basis, uint8_t schulter,
                              uint8_t ellbogen, uint8_t handgelenk, uint8_t greifer) {
    I2cFrameV1 frame;
    frame.start_a = I2C_FRAME_START_A;
    frame.start_b = I2C_FRAME_START_B;
    frame.protokoll_version = I2C_FRAME_VERSION;
    frame.sequenz = i2c_sequenz++;
    frame.flags = flags;
    frame.basis_soll = basis;
    frame.schulter_soll = schulter;
    frame.ellbogen_soll = ellbogen;
    frame.handgelenk_soll = handgelenk;
    frame.greifer_soll = greifer;
    frame.crc8 = i2c_crc_berechnen(&frame);

    Wire.beginTransmission(I2C_SLAVE_ADDR);
    Wire.write((const uint8_t*)&frame, sizeof(frame));
    Wire.endTransmission();
}

#endif
