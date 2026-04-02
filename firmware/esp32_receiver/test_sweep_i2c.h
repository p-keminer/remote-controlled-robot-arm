#ifndef TEST_SWEEP_I2C_H
#define TEST_SWEEP_I2C_H

// =============================================================================
// Test-Sweep-Generator — I2C-Kettentest ESP32 Receiver → Arduino
// =============================================================================
//
// Erzeugt ein Pendel-Signal das ueber i2c_frame_senden() an den Arduino gesendet
// wird. Dient ausschliesslich zum Testen der I2C-Kette — unabhaengig von ESP-NOW
// und IMU-Daten.
//
// Aktuell bewegen sich ALLE 5 ACHSEN mit unterschiedlichen Amplituden:
//   - Basis, Handgelenk, Greifer: volle Amplitude (±127 im Byte-Raum)
//   - Schulter: reduzierte Amplitude (±80) — mechanische Belastung,
//     Schulter traegt das Armgewicht
//   - Ellbogen: reduzierte Amplitude (±60) — volle Amplitude erzeugte
//     schwungartiges Hochfahren wenn Basis am Anschlag (alle Achsen synchron)
//
// Argumente-Reihenfolge: Flags, Basis, Schulter, Ellbogen, Handgelenk, Greifer.
//
// Siehe: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md (Abschnitte 5, 6, 12)
// =============================================================================

#include "i2c_frame.h"

// Mittelposition im 0-255 Byte-Raum.
// WICHTIG: Mappt auf die Servo-Mittelwerte (MIN+MAX)/2 im Arduino-Sketch.
// Wenn dieser Wert geaendert wird, muessen die Startwerte im Arduino-Sketch
// (ziel[] Array und neutral_setzen()) ebenfalls angepasst werden!
// Siehe: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md Abschnitt 5
#define SWEEP_NEUTRAL    128

// Voller Bereich: 128 ± 127 = Werte von 1 bis 255.
// Fuer kleinere Testbewegungen reduzieren (z.B. 13 fuer ~5% Bereich).
// ACHTUNG bei Amplitude > 127: sweep_offset muss int16_t sein (nicht int8_t)!
// Siehe: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md Abschnitt 6 (int8_t Bug)
#define SWEEP_AMPLITUDE  127

// 50 Hz Frame-Rate (alle 20ms). Bei I2C kommen alle Frames zuverlaessig an
// (kein Listen-Fenster-Problem wie bei SoftwareSerial). 50Hz passt zur
// Arduino-Loop-Rate (ebenfalls 50 Hz / 20ms) und ergibt feinste Aufloesung.
// WICHTIG: ESP32-Loop-Delay muss <= SWEEP_INTERVALL sein (aktuell 10ms).
#define SWEEP_INTERVALL  20

// Winkel-Inkrement pro Frame. Hoeher = schnellere Bewegung.
// Bei Schritt 1 und 50 Hz: voller Sweep in ~6.3 Sekunden (127/1 * 20ms * 2).
#define SWEEP_SCHRITT    1

// MUSS int16_t sein — NICHT int8_t!
// int8_t kann nur -128..127 halten. Bei Amplitude 127 + Schritt 2 tritt
// Overflow auf (126+2=128 → -128 als int8_t), was zu einem schlagartigen
// Sprung der Servo-Position fuehrt.
// Siehe: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md Abschnitt 6
static int16_t sweep_offset = 0;
static int16_t sweep_richtung = 1;
static unsigned long sweep_letztes_ms = 0;

static void test_sweep_update() {
    unsigned long jetzt = millis();
    if (jetzt - sweep_letztes_ms < SWEEP_INTERVALL) return;
    sweep_letztes_ms = jetzt;

    // Offset erhoehen/verringern und an den Grenzen die Richtung umkehren
    sweep_offset += sweep_richtung * SWEEP_SCHRITT;
    if (sweep_offset >= SWEEP_AMPLITUDE) {
        sweep_offset = SWEEP_AMPLITUDE;   // Clampen! Sonst uint8_t Overflow bei wert-Berechnung
        sweep_richtung = -1;
    }
    if (sweep_offset <= -SWEEP_AMPLITUDE) {
        sweep_offset = -SWEEP_AMPLITUDE;  // 128 + (-127) = 1 ist OK, 128 + (-128) = 0 waere Overflow!
        sweep_richtung = 1;
    }

    // Aktueller Wert: Neutral + Offset (ergibt 1..255 bei vollem Bereich)
    uint8_t wert = SWEEP_NEUTRAL + sweep_offset;

    // Schulter nur minimal bewegen (±80 statt voller Amplitude)
    int16_t schulter_offset = (sweep_offset * 80) / SWEEP_AMPLITUDE;
    uint8_t schulter_wert = SWEEP_NEUTRAL + schulter_offset;

    // Ellbogen reduziert (±60 statt voller Amplitude) — volle Amplitude
    // erzeugt schwungartiges Hochfahren wenn Basis am Anschlag
    int16_t ellbogen_offset = (sweep_offset * 60) / SWEEP_AMPLITUDE;
    uint8_t ellbogen_wert = SWEEP_NEUTRAL + ellbogen_offset;

    // Argumente: flags, basis, schulter, ellbogen, handgelenk, greifer
    i2c_frame_senden(FRAME_FLAG_VALID, wert, schulter_wert, ellbogen_wert, wert, wert);

    // Debug-Ausgabe alle 50 Frames (~1x pro Sekunde bei 50Hz)
    if (i2c_sequenz % 50 == 0) {
        Serial.printf("[SWEEP] wert=%d seq=%d\n", wert, i2c_sequenz);
    }
}

#endif
