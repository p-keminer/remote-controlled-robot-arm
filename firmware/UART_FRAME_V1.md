# UART Frame V1

## Zweck

Dieses Dokument beschreibt das minimale serielle Startformat zwischen `esp32_receiver` und `arduino_arm`.

## Aktueller Stand

Das Format ist konzeptionell festgelegt, aber noch nicht bench-validiert.
Es ist absichtlich klein, fest und leicht auswertbar, damit der erste `Receiver -> Arduino`-Pfad nicht durch zu viele Sonderfaelle kompliziert wird.

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
} UartFrameV1;
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

- ist in `../COMMUNICATION_FRAMEWORK.md` als kanonisches UART-Minimalformat gespiegelt
- wird spaeter von `esp32_receiver/` erzeugt und von `arduino_arm/` ausgewertet
- muss mit `../SAFETY_FRAMEWORK.md` konsistent bleiben
