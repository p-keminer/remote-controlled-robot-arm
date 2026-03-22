# Communication Framework

Dieses Dokument ist die kanonische Quelle fuer alle Kommunikationsregeln zwischen Controller, Receiver und Arduino.

## Kommunikationspfade

- IMUs und Multiplexer kommunizieren lokal ueber I2C mit dem Controller-ESP32.
- Der Flex-Sensor wird lokal ueber ADC am Controller erfasst.
- Controller und Receiver tauschen Bewegungsdaten lokal drahtlos ueber `ESP-NOW` aus.
- Receiver und Arduino kommunizieren ueber UART.

## Grundregeln

- Jede Kommunikationsstufe hat ein eigenes, klar beschriebenes Datenformat.
- Zielwerte werden als transportierbare Sollwerte und nicht als lose Interpretationen verschickt.
- Integritaetspruefungen sind verpflichtend, sobald Daten zwischen Mikrocontrollern uebertragen werden.
- Zeitverhalten, Timeout und Fehlerreaktion gehoeren zum Protokoll und nicht nur zur Implementierung.

## V1-Entscheidung

- `ESP-NOW` ist der einzige vorgesehene Funkpfad fuer v1.
- WLAN, Web-API, Cloud und allgemeine Fernverbindungen sind nicht Teil der v1-Kommunikation.

## Funkstrecke Controller -> Receiver

- Standardpfad ist lokales `ESP-NOW`.
- Der Steuerpfad soll nicht als offener Broadcast-Standardpfad behandelt werden.
- Frames muessen Protokollversion, Integritaet und Frischeannahmen beruecksichtigen.
- Unbekannte, veraltete oder ungueltige Frames duerfen nicht in Bewegung uebergehen.
- Aenderungen an Paketgroesse, Wertebereichen oder Integritaetslogik muessen hier und in `security/COMMUNICATION_SECURITY.md` gespiegelt werden.

## UART Receiver -> Arduino

- UART ist die bevorzugte Bruecke fuer Debugbarkeit und einfache Kopplung.
- Das serielle Frame enthaelt nur Daten, die fuer die Servoebene wirklich noetig sind.
- Der Receiver darf dem Arduino keine unvalidierten oder unvollstaendigen Bewegungsdaten uebergeben.
- Timeout und Neutralverhalten muessen auf beiden Seiten abgestimmt sein.

## ImuPaket v1 — ESP-NOW Paketformat (Stand 2026-03-22)

```
typedef struct {
    float heading;
    float roll;
    float pitch;
} SensorDaten;

typedef struct {
    uint8_t     protokoll_version;   // aktuell: 1
    uint32_t    zaehler;             // Frame-Zaehler fuer Frischeprüfung
    SensorDaten sensoren[2];         // Euler-Winkel beider IMUs
    uint8_t     pruefsumme;          // XOR ueber alle vorherigen Bytes
} ImuPaket;
```

- Paketgroesse: `sizeof(ImuPaket)` — Receiver verwirft Pakete anderer Groesse
- Pruefsumme: XOR aller Bytes ausser dem letzten Byte
- Frische: Pakete mit `zaehler <= letzter_zaehler` werden verworfen
- Sendeintervall: 50ms (20Hz)

## Fehlerfaelle

- ungueltige Integritaetspruefung -> Frame verwerfen
- unbekannter oder unfreigegebener Peer -> Frame verwerfen
- veraltetes oder doppeltes Frame -> nicht zur Bewegung verwenden
- ungueltige serielle Nutzlast -> nicht an die Servoebene durchreichen

## Nachweis

- Bench-, Security- und Integrationspruefungen fuer Kommunikationspfade werden unter `tests/` gepflegt.
- Messdaten und Beobachtungen zu Stabilitaet und Timing werden unter `docs/measurement_logs/` dokumentiert.
