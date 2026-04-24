# calibration/servo_limits

## Zweck

Dieser Ordner dokumentiert die mechanischen und softwareseitigen Winkelgrenzen aller Servos.

## Aktueller Stand

Erstmessung am 2026-04-02 durchgefuehrt. Alle 5 Servos auf 90° kalibriert, Shoulder-Servo (Originalservo defekt) durch Reserveservo ersetzt.

### Reale Pin-zu-Gelenk-Zuordnung

Die tatsaechliche Verkabelung weicht vom offiziellen Standard ab:

| Pin | Poti | Gelenk   | Offiziell erwartet |
|-----|------|----------|--------------------|
| D9  | A0   | Base     | Base               |
| D6  | A1   | Gripper  | Shoulder           |
| D5  | A2   | Wrist    | Elbow              |
| D3  | A3   | Elbow    | Wrist              |
| D11 | A6   | Shoulder | Gripper            |

### Gemessene Servo-Grenzwerte

| Gelenk   | Pin | Min  | Max  | Beschreibung                                          |
|----------|-----|------|------|-------------------------------------------------------|
| Base     | D9  | 12°  | 139° | 12° = max rechts (Uhrzeiger), 139° = max links (gegen Uhrzeiger) |
| Gripper  | D6  | 32°  | 126° | 32° = komplett offen, 126° = komplett geschlossen     |
| Wrist    | D5  | 5°   | 177° | 5° = Uhrzeiger, 177° = gegen Uhrzeiger                |
| Elbow    | D3  | 80°  | 175° | 80° = komplett gerade, 175° = komplett eingeklappt    |
| Shoulder | D11 | 35°  | 142° | 35° = komplett eingeklappt, 90° = aufrecht, 142° = max nach hinten |

### Hinweise

- Shoulder offizieller Potentiometer-Sketch begrenzte auf 35-90, realer Bereich ist 35-142 (107°)
- Shoulder-Servo ist der Reserveservo — Originalservo war defekt
- Neutralposition aller Servos: 90° (kalibriert am 2026-04-02)

## Inhalt

- Min-/Max-Winkel pro Servo
- Neutralpositionen und sichere Rueckfallwerte
- Hinweise auf mechanische Anschlaege oder kritische Bereiche
- spaetere Zuordnung der real verbauten Servos nach Stock-Aufbau
- Abgleich zwischen offizieller Stock-5-Servo-Basis und realem Kit-Inhalt

## Regeln

- jeder Grenzwert muss auf eine reale Beobachtung, Messung oder technische Vorgabe zurueckfuehrbar sein
- Komfortgrenzen und harte Sicherheitsgrenzen nicht vermischen
- Stock-Code-Annahmen duerfen nicht direkt als reale Grenzwerte uebernommen werden

## Schnittstellen/Abhaengigkeiten

- folgt `../README.md`, `../../SAFETY_FRAMEWORK.md` und `../../CALIBRATION_FRAMEWORK.md`
- wirkt direkt auf `../../firmware/arduino_arm/`

## Recherchequellen

- [Adeept V4 Package Review](../../official_downloads/ADEEPT_V4_PACKAGE_REVIEW.md) fuer die Einordnung `5 aktive Servos + 1 Reserve/Testservo`.
- [Servo90.ino](../../official_downloads/extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Code/_6%20Servo90/Servo90/Servo90.ino) fuer die offizielle 5-Servo-Initialbasis.
