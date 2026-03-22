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
- Broadcast- oder Gruppensteuerung ist kein zulaessiger Bewegungsstandardpfad.
- `XOR`, `CRC` oder reine Plausibilitaetspruefungen gelten nicht als Security-Massnahme.

## V1-Entscheidung

- `ESP-NOW` ist der einzige vorgesehene Funkpfad fuer v1.
- WLAN, Web-API, Cloud und allgemeine Fernverbindungen sind nicht Teil der v1-Kommunikation.

## Funkstrecke Controller -> Receiver

- Standardpfad ist lokales `ESP-NOW`.
- Der Steuerpfad soll nicht als offener Broadcast-Standardpfad behandelt werden.
- Frames muessen Protokollversion, Integritaet und Frischeannahmen beruecksichtigen.
- Frames muessen zusaetzlich einen Session-Bezug und einen applikationsseitigen Authentisierungstag tragen.
- Unbekannte, veraltete oder ungueltige Frames duerfen nicht in Bewegung uebergehen.
- MAC-seitiger Sendeerfolg reicht nicht; der Projektpfad braucht Anwendungs-ACK und definierte Duplicate-Logik.
- Aenderungen an Paketgroesse, Wertebereichen oder Integritaetslogik muessen hier und in `security/COMMUNICATION_SECURITY.md` gespiegelt werden.

## UART Receiver -> Arduino

- UART ist die bevorzugte Bruecke fuer Debugbarkeit und einfache Kopplung.
- Das serielle Frame enthaelt nur Daten, die fuer die Servoebene wirklich noetig sind.
- Der Receiver darf dem Arduino keine unvalidierten oder unvollstaendigen Bewegungsdaten uebergeben.
- Timeout und Neutralverhalten muessen auf beiden Seiten abgestimmt sein.

## ImuPaket v1 — Secure Draft fuer ESP-NOW (Stand 2026-03-22)

```
typedef struct {
    float heading;
    float roll;
    float pitch;
} SensorDaten;

typedef struct {
    uint8_t     protokoll_version;   // aktuell: 1
    uint8_t     msg_typ;             // motion, ack, arm, disarm, error
    uint16_t    flags;               // armed, degraded, calibrating, estop
    uint32_t    session_id;          // zufaellig pro Boot / Bind-Vorgang
    uint32_t    zaehler;             // monoton pro Session
    SensorDaten sensoren[2];         // Euler-Winkel beider IMUs
    uint16_t    flex_raw;            // Rohwert oder normalisierter Griffwert
    uint16_t    reserved;
    uint64_t    auth_tag64;          // gekuerzter MAC ueber alle vorherigen Bytes
} ImuPaketV1;
```

- Paketgroesse: `sizeof(ImuPaketV1)` — Receiver verwirft Pakete anderer Groesse
- Sessionbindung: `session_id` muss zur aktuellen bewaffneten Sitzung passen
- Frische: Pakete mit `zaehler <= letzter_zaehler` werden verworfen
- Authentisierung: `auth_tag64` wird applikationsseitig aus lokalem Schluesselmaterial berechnet
- Nur gueltige `msg_typ`- und `flags`-Kombinationen duerfen in Bewegungslogik uebergehen
- Sendeintervall: 50ms (20Hz)

Hinweis:

- Einfache XOR- oder CRC-Felder koennen zusaetzlich fuer Debug oder Korruptionserkennung existieren, ersetzen aber nicht den applikationsseitigen Authentisierungstag.

## Fehlerfaelle

- ungueltige Integritaetspruefung -> Frame verwerfen
- unbekannter oder unfreigegebener Peer -> Frame verwerfen
- veraltetes oder doppeltes Frame -> nicht zur Bewegung verwenden
- alte oder unerwartete `session_id` -> Frame verwerfen
- fehlender oder ungueltiger `auth_tag64` -> Frame verwerfen
- ungueltige serielle Nutzlast -> nicht an die Servoebene durchreichen

## Nachweis

- Bench-, Security- und Integrationspruefungen fuer Kommunikationspfade werden unter `tests/` gepflegt.
- Messdaten und Beobachtungen zu Stabilitaet und Timing werden unter `docs/measurement_logs/` dokumentiert.

## Recherchequellen

- [ESP-IDF ESP-NOW Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html) fuer Security-Grundlagen, Broadcast-Grenzen und die Empfehlung zu ACK- und Sequenzlogik.
- [ESP-NOW Integer Underflow Vulnerability Advisory](https://github.com/espressif/esp-idf/security/advisories/GHSA-hqhh-cp47-fv5g) fuer die Upgrade- und Parse-Haertungsanforderung auf Stack-Ebene.
- [Replay Attacks Vulnerability In ESP-NOW](https://github.com/espressif/esp-now/security/advisories/GHSA-wf6q-c2xr-77xj) fuer die zusaetzliche Motivation, Session- und Replay-Abwehr nicht nur der optionalen Komponente zu ueberlassen.
