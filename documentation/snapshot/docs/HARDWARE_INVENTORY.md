# Hardware Inventory

## Projektbezug

- Datum: 2026-04-24
- Bearbeiter: p-keminer
- Aufbauzustand: aktueller Arm-Prototyp mechanisch fertig, Controller auf Lochraster/Perfboard, Debug-Bridge und Digital Twin verifiziert
- Produktbasis: Adeept 5-DOF Robotic Arm Kit (Amazon B085C2S1N7), V4.0
- Referenzdokument: `hardware/ADEEPT_ARM_PRODUCT_BASELINE.md`

## Kernkomponenten — Arm

| Komponente | Soll | Ist | Zustand | Hinweis |
| --- | --- | --- | --- | --- |
| Adeept Roboterarm (Acrylstruktur) | 1 | 1 | montiert | Stock-Aufbau bestaetigt 2026-03-24, Fotos unter `docs/photos/` |
| Adeept Driver Board (ATmega328P) | 1 | 1 | funktionsfaehig | als Arduino UNO R3 programmiert |
| OLED Display 0.96'' | 1 | 1 | vorhanden | aktuell ausgesteckt (I2C-Bus fuer ESP32-Verbindung belegt) |
| AD002 Servo (verbaut) | 5 | 5 | funktionsfaehig | alle kalibriert auf 90° Neutral, Limits empirisch gemessen (2026-04-02) |
| AD002 Servo (Reserve) | 1 | 0 | verbraucht | Reserveservo jetzt als Shoulder verbaut (Original defekt) |
| Square Bearing Turntable | 1 | 1 | montiert | Basis-Drehplatte |
| Battery Holder (2x 18650) | 1 | 1 | in Benutzung | bestueckt mit 2x Molicel INR-18650-M35A |
| Micro USB Cable | 1 | 1 | vorhanden | fuer Upload und Diagnose |
| Servo Extension Cable | 1 | 1 | vorhanden | laut Packing List |
| Potentiometer (Drehregler) | 5 | 5 | verbaut | Stock-Steuerung auf dem Adeept-Board |

## Kernkomponenten — ESP32 Elektronik

| Komponente | Soll | Ist | Zustand | Hinweis |
| --- | --- | --- | --- | --- |
| ESP32-S3 DevKitC-1 N16R8 (Controller) | 1 | 1 | aktiv | Boardrevision v1.0, RGB auf GPIO48 |
| ESP32-S3 DevKitC-1 N16R8 (Receiver) | 1 | 1 | aktiv | Boardrevision v1.0, RGB auf GPIO48 |
| ESP32-S3 DevKitC-1 N16R8 (Bridge) | 1 | 1 | aktiv | Boardrevision v1.0, Debug-Beobachterpfad |
| ESP32-S3 DevKitC-1 N16R8 (Reserve) | 1 | 1 | Reserve | unbenutzt |
| BNO055 IMU (GY-BNO055 Clone, Senzooe) | 3 | 3 | funktionsfaehig | aktuelle Zuordnung: S0=Hand/Wrist, S1=Unterarm, S2=Oberarm; Mux-Kanaele 0/1/2 |
| PCA9548A I2C-Multiplexer | 1 | 3-4 | funktionsfaehig | 1 aktiv genutzt (Adresse 0x70), weitere als Reserve |
| Potentiometer 10k (Greifer-Eingabe) | 1 | 1 | aktiv | aktueller robuster Greifer-Pfad auf GPIO1 |
| Lochraster-/Perfboard-Traeger (Controller) | 1 | 1 | aktiv | aktueller Wearable-/Prototypstand |

## Zusatzmaterial

| Material | Soll | Ist | Hinweis |
| --- | --- | --- | --- |
| Breadboards | 2 | 2+ | Sender-Bench und Receiver-Bench |
| LEDs (diverse Farben) | 8+ | 8+ | Controller: 5 (aktuell bestaetigt: GPIO4 blau, GPIO5 rot, GPIO6 weiss, GPIO7 gruen, GPIO10 FAULT-LED), Receiver: 3, Bridge: 4 |
| Widerstaende 100 Ohm (LED-Vorwiderstand) | 8+ | 8+ | je 1 pro LED |
| Widerstand 10kOhm (Greifer-Pull-Down) | 1 | 1 | bestehender Pull-Down am Controller |
| Dupont-Kabel (M-M, M-F) | diverse | vorhanden | Bench-Verkabelung |
| USB-C Kabel | 3 | 3 | je 1 pro aktivem ESP32 |

## Stromversorgung

| Komponente | Soll | Ist | Zustand | Hinweis |
| --- | --- | --- | --- | --- |
| Molicel INR-18650-M35A (Flat Top, ohne Loetfahne) | 4 | 4 | in Benutzung | 2 im Batteriefach, 2 als Wechselakkus |
| XTAR VC4SL Ladegeraet | 1 | 1 | in Benutzung | USB-C Ladeeingang, QC3.0/PD optional |

## Servo-Zuordnung und Limits (bestaetigt 2026-04-02)

| Gelenk | Arduino-Pin | Poti-Eingang | Min | Max | Neutral |
| --- | --- | --- | --- | --- | --- |
| Base | D9 | A0 | 12° | 139° | 90° |
| Gripper | D6 | A1 | 32° | 126° | 90° |
| Wrist | D5 | A2 | 5° | 177° | 90° |
| Elbow | D3 | A3 | 80° | 175° | 90° |
| Shoulder | D11 | A6 | 35° | 142° | 90° |

Achtung: Pin-Zuordnung weicht vom Adeept-Standard ab (D6=Gripper statt Shoulder, D5=Wrist statt Elbow, D3=Elbow statt Wrist, D11=Shoulder statt Gripper).

## Bekannte Abweichungen

- Shoulder-Servo ist der ehemalige Reserveservo (Original war defekt)
- Keine Reserve-Servos mehr verfuegbar — Nachbeschaffung sinnvoll
- OLED aktuell ausgesteckt wegen I2C-Bus-Belegung durch ESP32-Arduino-Verbindung
- Servo-Pin-Zuordnung weicht vom Adeept-Standard ab (siehe Tabelle oben)
- Greifer-Eingabe laeuft aktuell ueber externes Potentiometer auf `GPIO1`, nicht ueber den fruehen Flex-Sensor-Prototyp

## Offene Punkte

- Ersatz-Servos fuer aufgebrauchte Reserve beschaffen
- aktuelle Perfboard-/Gehauseintegration weiter verstetigen
- Flat-Top-Passung im Adeept-Batteriefach dokumentieren (funktioniert, aber Toleranz eng)
