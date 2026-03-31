# Communication Framework

Dieses Dokument ist die kanonische Quelle fuer alle Kommunikationsregeln zwischen Controller, Receiver und Arduino.

## Kommunikationspfade

- IMUs und Multiplexer kommunizieren lokal ueber I2C mit dem Controller-ESP32.
- Der Flex-Sensor wird lokal ueber ADC am Controller erfasst.
- Controller und Receiver tauschen Bewegungsdaten lokal drahtlos ueber `ESP-NOW` aus (Steuerpfad).
- Controller und Bridge tauschen die gleichen Daten parallel per `ESP-NOW` aus (Debug-Pfad, 2. Peer).
- Bridge und Pi kommunizieren per WiFi und MQTT (Mosquitto, Port 1883, passwortgeschuetzt).
- Receiver und Arduino kommunizieren ueber UART.
- Controller und Receiver setzen WiFi-Kanal 1 explizit per esp_wifi_set_channel(). Die Bridge bezieht den Kanal vom Router per WiFi.begin() und laeuft dadurch ebenfalls auf Kanal 1, solange der Router auf Kanal 1 konfiguriert ist.

## Grundregeln

- Jede Kommunikationsstufe hat ein eigenes, klar beschriebenes Datenformat.
- Zielwerte werden als transportierbare Sollwerte und nicht als lose Interpretationen verschickt.
- Integritaetspruefungen sind verpflichtend, sobald Daten zwischen Mikrocontrollern uebertragen werden.
- Zeitverhalten, Timeout und Fehlerreaktion gehoeren zum Protokoll und nicht nur zur Implementierung.
- Broadcast- oder Gruppensteuerung ist kein zulaessiger Bewegungsstandardpfad.
- `XOR`, `CRC` oder reine Plausibilitaetspruefungen gelten nicht als Security-Massnahme.

## V1-Entscheidung

- `ESP-NOW` ist der einzige vorgesehene Funkpfad fuer den v1-Steuerpfad.
- WiFi wird ausschliesslich zwischen Bridge-ESP32 und Pi fuer das Entwicklungs-Dashboard genutzt.
- WiFi ist nicht Teil des sicherheitskritischen Steuerpfads (Controller → Receiver → Arduino).
- Web-API, Cloud und allgemeine Fernverbindungen sind nicht Teil der v1-Kommunikation.

## Funkstrecke Controller -> Receiver

- Standardpfad ist lokales `ESP-NOW`.
- Der Steuerpfad soll nicht als offener Broadcast-Standardpfad behandelt werden.
- Der aktuelle Bench-Pfad beruecksichtigt bereits Protokollversion, Integritaet, Frischeannahmen und Notaus-Flag und bildet drei IMUs mit Kalibrierungsstatus ab (ImuPaket v4).
- Der Produktivpfad fuer Realbetrieb muss zusaetzlich einen Session-Bezug und einen applikationsseitigen Authentisierungstag tragen.
- Unbekannte, veraltete oder ungueltige Frames duerfen nicht in Bewegung uebergehen.
- MAC-seitiger Sendeerfolg reicht nicht; der Projektpfad braucht Anwendungs-ACK und definierte Duplicate-Logik.
- Aenderungen an Paketgroesse, Wertebereichen oder Integritaetslogik muessen hier und in `security/COMMUNICATION_SECURITY.md` gespiegelt werden.

## UART Receiver -> Arduino

- UART ist die bevorzugte Bruecke fuer Debugbarkeit und einfache Kopplung.
- Das serielle Frame enthaelt nur Daten, die fuer die Servoebene wirklich noetig sind und bleibt bewusst klein.
- Der Receiver darf dem Arduino keine unvalidierten oder unvollstaendigen Bewegungsdaten uebergeben.
- Timeout und Neutralverhalten muessen auf beiden Seiten abgestimmt sein.
- Das feste Minimalformat fuer den ersten Projektstand ist in `firmware/UART_FRAME_V1.md` beschrieben.

## ImuPaket v1 - historischer Bench-Stand (2026-03-22, abgeloest)

```c
typedef struct {
    float heading;
    float roll;
    float pitch;
} SensorDaten;

typedef struct __attribute__((packed)) {
    uint32_t    zaehler;
    SensorDaten sensoren[2];
    float       flex_prozent;
    uint8_t     protokoll_version;  // 1
    uint8_t     pruefsumme;
} ImuPaketV1Bench;
```

- Abgeloest durch ImuPaket v4 (siehe unten)
- Archiviert unter `firmware/espnow_imu_v1/` und `firmware/espnow_receiver_v1/`

## ImuPaket v4 - aktueller Bench-Stand fuer ESP-NOW (Stand 2026-03-28)

```c
typedef struct {
    float heading;
    float roll;
    float pitch;
} SensorDaten;

typedef struct {
    uint8_t sys;
    uint8_t gyro;
    uint8_t accel;
    uint8_t mag;
} KalibStatus;

typedef struct __attribute__((packed)) {
    uint32_t    zaehler;
    SensorDaten sensoren[3];        // Oberarm, Unterarm, Hand/Wrist
    KalibStatus kalib[3];           // Kalibrierungsstatus pro Sensor (0-3 je Achse)
    float       flex_prozent;       // -1.0 wenn Sensor unplausibel
    uint8_t     flags;              // Bitfeld: Bit 0 = FLAG_NOTAUS
    uint8_t     protokoll_version;  // 4
    uint8_t     pruefsumme;
} ImuPaketV4Bench;
```

- Paketgroesse: `sizeof(ImuPaketV4Bench)` — Receiver verwirft Pakete anderer Groesse
- Integritaet im Bench-Pfad: XOR-Pruefsumme ueber alle Bytes bis auf das Pruefsummenfeld
- Frische: Pakete mit `zaehler <= letzter_zaehler` werden verworfen
- Sendeintervall: 50ms (20Hz)
- Drei IMUs ueber PCA9548A Mux-Kanaele 0/1/2, Mux-Delay 10ms
- Kalibrierungsoffsets persistent im ESP32-NVS, automatisches Laden beim Boot
- Einzelkalibrierung per Serial-Befehl (CAL0/CAL1/CAL2, RECAL, STOP)
- Live-Sensorausfallerkennung fuer IMUs und Flex-Sensor mit automatischer Wiederherstellung
- LED-Debugging invertiert (aus=OK, an=Problem): 5 LEDs + RGB am Controller, 2 LEDs + RGB am Receiver, 3 LEDs + RGB an Bridge
- Multi-Peer ESP-NOW: Controller sendet an Receiver (Steuerpfad) und Bridge (Debug-Pfad) gleichzeitig
- Bridge validiert Pakete (Groesse, Absender-MAC, Pruefsumme, Protokollversion) und publiziert als JSON per MQTT
- **Notaus (Emergency Stop):** Bit 0 im `flags`-Feld signalisiert Notaus. Der Controller liest einen Toggle-Button an GPIO21 (INPUT_PULLUP, 50ms Entprellung). Jeder Tastendruck toggelt den Notaus-Zustand. Receiver und Bridge werten das Flag aus und zeigen Notaus visuell an (RGB orange blinkend).
- Dieser Stand ist als Bench-Zwischenstufe fuer Sensor- und Funkvalidierung zulaessig, aber nicht als fertige Security-Baseline fuer Realbetrieb
- Archiviert unter `firmware/espnow_imu_v2/` (ohne Persistenz) und aktuell in `firmware/esp32_controller/`

## Debug-Bridge-Pfad (Entwicklungswerkzeug)

Die Bridge empfaengt identische ImuPaket v4 Frames wie der Receiver, validiert sie und konvertiert sie zu kompaktem JSON fuer MQTT.

- **Protokoll:** MQTT ueber WiFi (PubSubClient), Mosquitto-Broker auf dem Pi (Port 1883)
- **Authentifizierung:** Passwortgeschuetzt (`allow_anonymous false`), eigener MQTT-User fuer Bridge
- **Topics:** `robotarm/imu` (20Hz, QoS 0), `robotarm/status` (1Hz, retained), `robotarm/kalib` (bei Aenderung, retained), `robotarm/ota/log`
- **Kanal-Alignment:** Alle ESPs (Controller, Receiver, Bridge) muessen auf dem gleichen WiFi-Kanal laufen (aktuell Kanal 1)
- **Sicherheit:** Die Bridge ist rein beobachtend. Sie kann keine Steuerbefehle senden. Ein Ausfall der Bridge hat keinen Einfluss auf den Steuerpfad.
- **JSON-Format:** Kompakte Schluessel (`z`=zaehler, `s`=sensoren, `h`=heading, `r`=roll, `p`=pitch, `k`=kalib, `f`=flex_prozent, `fl`=flags, `notaus`=Notaus-Status, `v`=protokoll_version)
- **Detaillierte Topic-Struktur:** Siehe `dashboard/DASHBOARD_CONCEPT.md`

## Naechste Paketrevision vor dem Security-Uplift

- `flags` Bitfeld ist seit v4 vorhanden (Bit 0 = Notaus); weitere Bits fuer Sensor-Gueltigkeits- oder Quellenmasken koennen ergaenzt werden
- wenn das Paket erneut angefasst wird, soll ein klar reservierter Erweiterungsbereich fuer spaetere Security-Felder mitgeplant werden
- der eigentliche Security-Uplift wird erst nach erster `Receiver -> Arduino`-Grundkette aktiviert

## ImuPaket v2 - Security-Zielbild vor Realbetrieb

```c
typedef struct {
    float heading;
    float roll;
    float pitch;
} SensorDaten;

typedef struct {
    uint8_t     protokoll_version;   // fuer den Security-Uplift als neue Version einplanen, z.B. 2
    uint8_t     msg_typ;             // motion, ack, arm, disarm, error
    uint16_t    flags;               // armed, degraded, calibrating, estop
    uint32_t    session_id;          // zufaellig pro Boot / Bind-Vorgang
    uint32_t    zaehler;             // monoton pro Session
    SensorDaten sensoren[3];         // Zielbild: drei Segmentsensoren
    uint16_t    flex_raw;            // Rohwert oder normalisierter Griffwert
    uint16_t    reserved;
    uint64_t    auth_tag64;          // gekuerzter MAC ueber alle vorherigen Bytes
} ImuPaketV2Secure;
```

- Paketgroesse: `sizeof(ImuPaketV2Secure)` — Receiver verwirft Pakete anderer Groesse
- Sessionbindung: `session_id` muss zur aktuellen bewaffneten Sitzung passen
- Frische: Pakete mit `zaehler <= letzter_zaehler` werden verworfen
- Authentisierung: `auth_tag64` wird applikationsseitig aus lokalem Schluesselmaterial berechnet
- Nur gueltige `msg_typ`- und `flags`-Kombinationen duerfen in Bewegungslogik uebergehen
- Sendeintervall: 50ms (20Hz)
- Dieser Stand ist das dokumentierte Zielbild fuer Bewegungsfreigabe ausserhalb des reinen Bench-Betriebs

Hinweis:

- Einfache XOR- oder CRC-Felder koennen zusaetzlich fuer Debug oder Korruptionserkennung existieren, ersetzen aber nicht den applikationsseitigen Authentisierungstag.
- Das aktuell implementierte `ImuPaket v4` bleibt bis zum Security-Uplift ein Bench-Artefakt und ist kein Produktivfreigabekriterium.

## UART-Frame v1 - Minimaler Startstand fuer Receiver -> Arduino

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

- Das Format bleibt absichtlich klein, fest und binaerisch.
- `basis_soll` bis `greifer_soll` sind im ersten Startstand bewusst abstrahierte Achs-Sollwerte und keine direkt hart codierten Servopulse.
- Die oberen Bits in `flags` bleiben fuer spaetere Erweiterungen reserviert.
- Erst nach funktionierender Grundkette wird entschieden, ob weitere Diagnose- oder Safety-Felder wirklich noetig sind.

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
