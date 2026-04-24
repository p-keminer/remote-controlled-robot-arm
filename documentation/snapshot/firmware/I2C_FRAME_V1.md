# I2C Frame V1

## Zweck

Dieses Dokument beschreibt das minimale Binaerformat fuer die I2C-Kommunikation zwischen `esp32_receiver` und `arduino_arm`.

## Aktueller Stand

Das Format ist bench-validiert (bestaetigt 2026-04-02).
Transport: I2C (ESP32 GPIO13/14 als Master → Arduino A4/A5 als Slave, Adresse 0x42).
SoftwareSerial (UART) wurde durch I2C ersetzt, weil PCINT-Interrupts mit Timer1 (Servo-PWM) auf dem ATmega328P kollidieren und Servo-Jitter verursachen. I2C (TWI-Hardware) hat dieses Problem nicht.
Das Frame-Format selbst ist identisch geblieben: 11 Bytes, festes Binaerformat, gleiche Struktur.
Implementierung: `esp32_receiver/i2c_frame.h` (Sender) und `arduino_arm/arduino_arm_i2c.ino` (Empfaenger).
Details zur I2C-Migration: `SERVO_UART_DESIGNENTSCHEIDUNGEN.md` Abschnitt 13.

## Frame-Aufbau

```c
typedef struct __attribute__((packed)) {
    uint8_t start_a;            // 0xA5
    uint8_t start_b;            // 0x5A
    uint8_t protokoll_version;  // 1
    uint8_t sequenz;            // 0..255, danach Ueberlauf
    uint8_t flags;              // Datenstatus, Neutralanforderung, degraded, estop
    uint8_t basis_soll;         // 0..255, abstrahierter Sollwert
    uint8_t schulter_soll;      // 0..255, abstrahierter Sollwert
    uint8_t ellbogen_soll;      // 0..255, abstrahierter Sollwert
    uint8_t handgelenk_soll;    // 0..255, abstrahierter Sollwert
    uint8_t greifer_soll;       // 0..255, abstrahierter Sollwert
    uint8_t crc8;               // Integritaetspruefung ueber alle vorherigen Bytes
} I2cFrameV1;
```

## Feldbedeutung

- `start_a` und `start_b` markieren den Beginn eines gueltigen Frames
- `protokoll_version` erlaubt spaetere kontrollierte Erweiterungen
- `sequenz` hilft, doppelte oder aus der Reihenfolge laufende Frames sichtbar zu machen
- `flags` traegt den aktuellen Betriebs- und Sicherheitszustand
- `basis_soll` bis `greifer_soll` tragen die abstrahierten Achs-Sollwerte fuer den ersten Projektstand
- `crc8` deckt einfache Leitungs- oder Parsefehler ab

## Flags im Startstand

- Bit 0: Daten gueltig
- Bit 1: Neutralposition anfordern
- Bit 2: Degraded Mode
- Bit 3: E-Stop / harte Sperre
- Bit 4 bis 7: reserviert fuer spaetere Erweiterungen

## Regeln

- Das Format bleibt bis zur ersten stabilen Grundkette fest und variablenfrei.
- Keine Textkommandos, keine variablen Laengen und keine verschachtelten Nutzlasten im Startstand.
- Die Achsfelder bleiben bewusst abstrahiert und sind noch keine direkt hart codierten Servopulse.
- Erweiterungen duerfen nur kontrolliert ueber `protokoll_version` und reservierte Bits erfolgen.

## Schnittstellen/Abhaengigkeiten

- ist in `../COMMUNICATION_FRAMEWORK.md` als kanonisches I2C-Minimalformat gespiegelt
- wird von `esp32_receiver/i2c_frame.h` erzeugt und von `arduino_arm/arduino_arm_i2c.ino` ueber I2C empfangen
- muss mit `../SAFETY_FRAMEWORK.md` konsistent bleiben
