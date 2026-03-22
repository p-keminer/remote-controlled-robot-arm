# IMU Robotic Arm

Dieses Repository ist das dokumentationsgetriebene Grundgeruest fuer einen IMU-gesteuerten 5-DOF-Roboterarm auf Basis eines Adeept-Arms.
Das Projekt wird unter WSL entwickelt und trennt bewusst zwischen Vorbereitung, Security, Hardware, Kalibrierung, Firmware und Tests.

## Projektziel

Gebaut werden soll ein Roboterarm, der Bewegungen des menschlichen Arms ueber mehrere BNO055-IMUs und einen Flex-Sensor in Servo-Zielwinkel uebersetzt.
Die erste Version wird als lokal betriebenes Embedded-System vorbereitet: `ESP-NOW` zwischen den ESP32, `UART` zum Arduino, kein WLAN und keine Cloud-Anbindung in v1.

## Aktueller Fokus

Die aktuelle Phase konzentriert sich auf:

- LED-Debugging am Controller (GPIO4/5/6 IMU-Status, GPIO7 COMMS, GPIO10 FAULT) und Receiver (GPIO4/5/6)
- Buzzer-Pfad GPIO21 mit sicherem Default-Off pruefen
- Roboterarm aufbauen und UART-Pfad Receiver → Arduino in Betrieb nehmen
- Bench-Kommunikation von `ImuPaket v1` auf die dokumentierte Security-Baseline mit Session- und Authentisierungsschicht anheben

Abgeschlossen (Stand 2026-03-22):

- Toolchain vollstaendig eingerichtet: Arduino IDE 3.3.7 als Hauptumgebung, PlatformIO als lokaler Fallback- und Gegencheckpfad
- BNO055 Einzelsensor, PCA9548A-Mux und zwei simultane Sensoren validiert
- Flex-Sensor ADC-Pfad ausgelesen und Rohwerte dokumentiert
- IMU-Daten per ESP-NOW Unicast uebertragen: `ImuPaket v1` als Bench-Paket mit XOR-Pruefsumme und Frische-Check

## Leitdokumente

Vor groesseren Aenderungen sind besonders relevant:

- `README.md` als Einstieg
- `CLAUDE.md` als kurzer KI-Brief
- `PROJEKT_ABLAUFPLAN.md` fuer die Reihenfolge der Projektphasen
- `PROJEKT_FORTSCHRITT.md` fuer den aktuellen Projektstand
- `GLOBAL_RULES.md` fuer Entwicklungs- und Dokumentationsregeln
- `ARCHITECTURE.md` fuer Systemgrenzen und Datenfluss
- `SYSTEM_FRAMEWORKS.md` fuer feste Subsysteme und Verantwortungen
- `COMMUNICATION_FRAMEWORK.md` fuer `ESP-NOW`-, Paket- und UART-Regeln
- `SECURITY_FRAMEWORK.md` fuer Security-Grundsaetze
- `CALIBRATION_FRAMEWORK.md` fuer Nullpunkt-, Referenz- und Mappingregeln
- `SAFETY_FRAMEWORK.md` fuer Limits, Watchdog, Neutralposition und Freigaben
- `preparation/README.md`, `security/README.md`, `hardware/README.md`, `firmware/README.md`, `tests/README.md`, `docs/README.md` und `future/README.md` fuer lokale Arbeitsbereiche

## Wichtige Befehle

Dokumentation sammeln und aktualisieren:

```bash
bash ./scripts/update_docs.sh
```

## Dokumentationsprinzip

Jeder relevante Ordner besitzt mindestens eine eigene `README.md`.
Groessere Unterprojekte besitzen zusaetzlich eine eigene `ROADMAP.md`.
Der Ordner `documentation/` ist ein automatisch erzeugter Snapshot und keine manuell gepflegte Quelldokumentation.
Der Ordner `docs/` bleibt die manuell gepflegte Arbeits- und Nachweisdokumentation.
Forschungs-, Konzept- und Entscheidungsdokumente muessen die verwendeten externen Quellen in einem eigenen Abschnitt `Recherchequellen` auffuehren.
Nicht-repotaugliche lokale Werte wie Schluessel, Peer-Listen, lokale Identitaetsdaten, API-Schluessel und absolute Dateipfade (z.B. `/home/username/`, `/mnt/c/Users/username/`) gehoeren nach `security/local/` oder in gitignorierte `*.local.*`-Konfigurationsdateien und duerfen niemals eingecheckt werden.

## Aktueller Entwicklungsstand

Dokumentations- und Prozessbasis ist angelegt.
Toolchain steht: Arduino IDE 3.3.7 ist die Hauptumgebung, PlatformIO bleibt als lokaler Fallback und Gegencheck erhalten.
Sensorpfad ist bench-validiert: BNO055 (Einzel und Dual ueber PCA9548A-Mux), Flex-Sensor und `ESP-NOW` Unicast mit `ImuPaket v1` als Bench-Zwischenstand.
Die dokumentierte Security-Baseline mit `session_id`, applikationsseitigem Authentisierungstag und Advisory-gepruefter Stack-Basis ist vorbereitet, aber noch nicht in die Firmware uebernommen.
Naechster Schritt: LED-Debugging, Buzzer-Pfad, Roboterarm-Aufbau und UART-Inbetriebnahme zum Arduino.
Zusaetzliche Post-v1-Ausbaurichtungen werden gesammelt unter `future/FUTURE_WORK.md`.
