# Communication Framework

Dieses Dokument ist die kanonische Quelle fuer alle Kommunikationsregeln zwischen Controller, Receiver, Bridge, Dashboard, ROS 2 und Arduino.

## Kommunikationspfade

- IMUs und Multiplexer kommunizieren lokal ueber I2C mit dem Controller-ESP32.
- Die aktuelle Greifer-Eingabe wird lokal ueber ADC am Controller erfasst.
- Controller und Receiver tauschen Bewegungsdaten lokal drahtlos ueber `ESP-NOW` aus.
- Controller und Bridge tauschen dieselben Daten parallel per `ESP-NOW` aus.
- Bridge und Pi kommunizieren per WiFi und MQTT.
- Receiver und Arduino kommunizieren ueber I2C (ESP32 als Master, Arduino als Slave an Adresse `0x42`).
- ROS 2 konsumiert den MQTT-Datenstrom beobachtend ueber `mqtt_bridge.py`.

## Grundregeln

- Jede Kommunikationsstufe hat ein eigenes, klar beschriebenes Datenformat.
- Steuerpfad und Debug-/Twin-Pfad sind getrennt zu betrachten.
- Integritaetspruefungen und Frischeannahmen gehoeren zum Protokoll.
- `XOR`, `CRC` oder reine Plausibilitaetspruefungen gelten nicht als Security-Massnahme.

## V1-Entscheidung

- `ESP-NOW` ist der einzige vorgesehene Funkpfad im Steuerpfad.
- WiFi wird ausschliesslich fuer die beobachtende Debug-Bridge zum Pi genutzt.
- Web-API, Cloud und allgemeine Fernverbindungen sind kein Teil der Bewegungsfreigabe.

## ImuPaket v4 - aktueller Arbeitsstand

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
    SensorDaten sensoren[3];        // S0 = Hand/Wrist, S1 = Unterarm, S2 = Oberarm
    KalibStatus kalib[3];
    float       flex_prozent;       // aktueller Greifer-/Grip-Wert ueber Feldname `f`
    uint8_t     flags;              // Bit 0 = Notaus
    uint8_t     protokoll_version;  // 4
    uint8_t     pruefsumme;
} ImuPaketV4Bench;
```

- Paketgroesse: `sizeof(ImuPaketV4Bench)`
- Integritaet im Bench-/Debug-Pfad: XOR-Pruefsumme
- Frische: Pakete mit ruecklaeufigem `zaehler` werden verworfen
- Sendeintervall: 5 ms
- Das Feld `flex_prozent` bzw. JSON-Schluessel `f` bleibt aus Rueckwaertskompatibilitaet bestehen, transportiert aktuell aber den Potentiometer-basierten Greiferwert

## Debug-Bridge-Pfad

- Bridge validiert ESP-NOW-Pakete und publiziert JSON per MQTT
- Topics: `robotarm/imu`, `robotarm/status`, `robotarm/kalib`, `robotarm/ota/log`
- Dashboard, MQTT MCP Server und ROS 2 lesen denselben beobachtenden Strom
- Ein Ausfall der Bridge darf den Steuerpfad nicht beeinflussen

## I2C Receiver -> Arduino

- I2C wurde statt UART gewaehlt, weil SoftwareSerial mit der Servo-Ebene kollidiert.
- ESP32 ist I2C-Master (`GPIO13`/`GPIO14`), Arduino ist I2C-Slave (`0x42`).
- Das Frame-Format bleibt klein, fest und binaerisch (`firmware/I2C_FRAME_V1.md`).
- Timeout- und Neutralverhalten muessen auf Receiver- und Arduino-Seite abgestimmt sein.

## Naechste Paketrevision vor dem Security-Uplift

- `flags` und `protokoll_version` bleiben als sichtbare Erweiterungspunkte verpflichtend.
- Vor Realbetrieb braucht der Pfad zusaetzlich `session_id`, monotonen Zaehler und einen applikationsseitigen Authentisierungstag.
- Paket- und Topic-Aenderungen muessen immer auch in `security/` und den Twin-Dokumenten gespiegelt werden.
