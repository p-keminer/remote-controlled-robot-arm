# IMU Robotic Arm

Dieses Repository ist das dokumentationsgetriebene Grundgeruest fuer einen IMU-gesteuerten 5-DOF-Roboterarm auf Basis eines Adeept-Arms.
Das Projekt wird unter Windows und WSL entwickelt und trennt bewusst zwischen Vorbereitung, Security, Hardware, Kalibrierung, Firmware und Tests.

## Lizenz

Die projekt-eigenen Inhalte dieses Repositories stehen unter **Apache License 2.0**.
Die verbindliche Lizenz steht in `LICENSE.md`.
Sicherheits- und Einsatzhinweise stehen getrennt in `SAFETY_NOTICE.md`.
Hersteller- und Drittmaterialien, insbesondere unter `official_downloads/`, bleiben zusaetzlich ihren eigenen Rechten und Hinweisen unterworfen.

## Projektziel

Gebaut werden soll ein Roboterarm, der Bewegungen des menschlichen Arms ueber mehrere BNO055-IMUs und einen Flex-Sensor in Servo-Zielwinkel uebersetzt.
Die erste Version wird als lokal betriebenes Embedded-System vorbereitet: `ESP-NOW` zwischen den ESP32, `UART` zum Arduino, kein WLAN und keine Cloud-Anbindung in v1.

## Aktueller Fokus

Die aktuelle Phase konzentriert sich auf:

- Stock-Baseline-Test des aufgebauten Adeept-Arms durchfuehren und dokumentieren
- Dashboard-Views im bestehenden IoT Control Center auf dem Pi implementieren
- UART-Pfad Receiver → Arduino in Betrieb nehmen
- Security-Uplift nach erster UART-Grundkette

Abgeschlossen (Stand 2026-03-27):

- Toolchain vollstaendig eingerichtet: Arduino IDE 3.3.7 als Hauptumgebung, PlatformIO als lokaler Fallback
- 3x BNO055 ueber PCA9548A-Mux bench-validiert, Flex-Sensor ADC-Pfad kalibriert
- `ImuPaket v3` per ESP-NOW Unicast: drei IMUs, Kalibrierungsstatus, XOR-Pruefsumme, Frische-Check
- BNO055-Kalibrierungspersistenz im NVS mit Einzelkalibrierung (CAL0/CAL1/CAL2)
- LED-Schema invertiert (aus=OK, blinken=Problem) mit RGB GPIO48 als FAULT auf allen ESPs
- Live-Sensorausfallerkennung fuer IMUs und Flex-Sensor
- Multi-Peer ESP-NOW: Controller sendet an Receiver (Steuerpfad) und Bridge (Debug-Pfad)
- Bridge-ESP32: ESP-NOW Empfang → WiFi/MQTT → Mosquitto (Pi) mit OTA und Passwort-Auth
- WiFi-Kanal 1 auf allen ESPs fuer ESP-NOW/WiFi-Koexistenz
- MQTT MCP Server fuer Claude Live-Sensorzugriff (6 Tools)
- Kompletter Datenpfad validiert: Controller → ESP-NOW → Bridge → MQTT → Pi → MCP → Claude
- Secret-Scanner mit 10 Kategorien, Pre-Commit/Pre-Push Hooks, GitHub Actions
- Adeept 5-DOF Roboterarm mechanisch im Stock-Zustand aufgebaut (noch nicht eingeschaltet/getestet)

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
- `hardware/ADEEPT_ARM_PRODUCT_BASELINE.md` fuer die konkrete Produktbasis des vorhandenen Adeept-Kits
- `hardware/electronics/POWER_SUPPLY_CONCEPT.md` fuer die Stromversorgungsstrategie von Stock-Test, Bench und spaeterem Projektbetrieb
- `official_downloads/README.md` fuer den importierten offiziellen Herstellerstand
- `LICENSE.md` fuer die Apache-2.0-Lizenz der projekt-eigenen Inhalte
- `SAFETY_NOTICE.md` fuer die getrennten Sicherheits- und Einsatzhinweise

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
Der Snapshot unter `documentation/` sammelt nur projektgepflegte Markdown-Dokumente; Vendor-Archive unter `official_downloads/raw/` und `official_downloads/extracted/` bleiben bewusst ausserhalb dieses Verwaltungsstands.
Forschungs-, Konzept- und Entscheidungsdokumente muessen die verwendeten externen Quellen in einem eigenen Abschnitt `Recherchequellen` auffuehren.
Nicht-repotaugliche lokale Werte wie Schluessel, Peer-Listen, lokale Identitaetsdaten, API-Schluessel und absolute Dateipfade (z.B. `/home/username/`, `/mnt/c/Users/username/`) gehoeren nach `security/local/` oder in gitignorierte `*.local.*`-Konfigurationsdateien und duerfen niemals eingecheckt werden.

## Aktueller Entwicklungsstand

Dokumentations- und Prozessbasis ist angelegt und wird nach jedem Meilenstein synchronisiert.
Toolchain steht: Arduino IDE 3.3.7 als Hauptumgebung, PlatformIO als Fallback.
Sensorpfad vollstaendig bench-validiert: drei BNO055 ueber PCA9548A-Mux, Flex-Sensor und `ESP-NOW` Unicast mit `ImuPaket v3` (Kalibrierungspersistenz im NVS).
Kommunikation: Controller sendet per ESP-NOW an Receiver (Steuerpfad) und Bridge (Debug-Pfad) gleichzeitig auf Kanal 1.
Debug-Infrastruktur: Bridge-ESP32 leitet Daten per WiFi/MQTT an Mosquitto (Pi); MQTT MCP Server erlaubt Claude direkten Live-Sensorzugriff.
LED-Schema invertiert (aus=OK, blinken=Problem) mit RGB GPIO48 als FAULT auf allen ESPs.
Secret-Scanner mit 10 Kategorien und automatischen Git-Hooks schuetzt vor versehentlichem Secret-Push.
Die Security-Baseline wird bewusst erst nach erster UART-Grundkette aktiviert.
Naechste Schritte: Stock-Baseline-Test, Dashboard-Views, UART-Inbetriebnahme zum Arduino.
