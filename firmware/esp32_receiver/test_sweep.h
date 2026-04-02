#ifndef TEST_SWEEP_H
#define TEST_SWEEP_H

// =============================================================================
// Test-Sweep-Generator — UART-Kettentest ESP32 Receiver → Arduino
// =============================================================================
//
// Erzeugt ein Pendel-Signal das ueber uart_frame_senden() an den Arduino gesendet
// wird. Dient ausschliesslich zum Testen der UART-Kette — unabhaengig von ESP-NOW
// und IMU-Daten.
//
// Aktuell bewegen sich: Gripper + Handgelenk (synchron).
// Alle anderen Achsen stehen auf SWEEP_NEUTRAL (Mittelposition).
//
// Um weitere Achsen zu testen, den entsprechenden SWEEP_NEUTRAL im
// uart_frame_senden()-Aufruf durch 'wert' ersetzen.
// Argumente-Reihenfolge: Basis, Schulter, Ellbogen, Handgelenk, Greifer.
//
// Siehe: firmware/SERVO_UART_DESIGNENTSCHEIDUNGEN.md (Abschnitte 5, 6)
// =============================================================================

#include "uart_frame.h"

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

// 20 Hz Frame-Rate. Muss hoch genug sein damit der Arduino bei seinem
// 60% Listen-Duty-Cycle (150ms von 250ms Zyklus) genuegend Frames empfaengt.
// Bei zu niedriger Rate: ruckelige Bewegung weil Frames verpasst werden.
#define SWEEP_INTERVALL  50

// Winkel-Inkrement pro Frame. Hoeher = schnellere Bewegung.
// Bei Schritt 2 und 20 Hz: voller Sweep in ~6.3 Sekunden (127/2 * 50ms * 2).
#define SWEEP_SCHRITT    2

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
    if (sweep_offset >= SWEEP_AMPLITUDE) sweep_richtung = -1;
    if (sweep_offset <= -SWEEP_AMPLITUDE) sweep_richtung = 1;

    // Aktueller Wert: Neutral + Offset (ergibt 1..255 bei vollem Bereich)
    uint8_t wert = SWEEP_NEUTRAL + sweep_offset;

    // Frame senden: Gripper + Handgelenk bewegen sich, Rest auf Neutral.
    // Argumente: flags, basis, schulter, ellbogen, handgelenk, greifer
    // Um eine andere Achse zu testen: SWEEP_NEUTRAL durch 'wert' ersetzen.
    uart_frame_senden(FRAME_FLAG_VALID, SWEEP_NEUTRAL, SWEEP_NEUTRAL, SWEEP_NEUTRAL, wert, wert);

    // Debug-Ausgabe alle 20 Frames (~1x pro Sekunde bei 20Hz)
    if (uart_sequenz % 20 == 0) {
        Serial.printf("[SWEEP] wert=%d seq=%d\n", wert, uart_sequenz);
    }
}

#endif
