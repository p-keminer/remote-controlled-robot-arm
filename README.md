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

- Roboterarm aufbauen und UART-Pfad Receiver → Arduino in Betrieb nehmen
- Bench-Kommunikation nach erster UART-Grundkette von `ImuPaket v3` auf die dokumentierte Security-Baseline mit Session- und Authentisierungsschicht anheben

Abgeschlossen (Stand 2026-03-26):

- Toolchain vollstaendig eingerichtet: Arduino IDE 3.3.7 als Hauptumgebung, PlatformIO als lokaler Fallback- und Gegencheckpfad
- BNO055 Einzelsensor, PCA9548A-Mux und drei simultane Sensoren validiert (Mux-Kanaele 0/1/2)
- Flex-Sensor ADC-Pfad ausgelesen und Rohwerte dokumentiert
- IMU-Daten per ESP-NOW Unicast uebertragen: `ImuPaket v3` mit drei IMUs, Kalibrierungsstatus, XOR-Pruefsumme und Frische-Check
- BNO055-Kalibrierungsoffsets persistent im NVS gespeichert mit Einzelkalibrierungsmodus (CAL0/CAL1/CAL2)
- LED-Debugging bench-validiert: Controller Ampelsystem (Gruen/Gelb/Rot + Blau COMMS + Weiss FAULT), Receiver (Gruen LINK + Blau UART + Gelb FAULT)
- Live-Sensorausfallerkennung fuer IMUs und Flex-Sensor mit automatischer Wiederherstellung

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

Dokumentations- und Prozessbasis ist angelegt.
Toolchain steht: Arduino IDE 3.3.7 ist die Hauptumgebung, PlatformIO bleibt als lokaler Fallback und Gegencheck erhalten.
Sensorpfad ist bench-validiert: drei BNO055 ueber PCA9548A-Mux, Flex-Sensor und `ESP-NOW` Unicast mit `ImuPaket v3` (Kalibrierungsstatus und NVS-Persistenz).
Der offizielle Adeept-V4.0-Download mit Tutorial-PDFs, Schaltplan und Originalcode liegt jetzt strukturiert unter `official_downloads/` und ist gegen Produktbasis, Servoannahmen und Stromversorgung ausgewertet.
Als aktuelle Beschaffungsbasis fuer den Stock- und Projekt-Strompfad sind `4x Molicel INR-18650-M35A` ohne Loetfahne plus `1x XTAR VC4SL` dokumentiert.
Die dokumentierte Security-Baseline mit `session_id`, applikationsseitigem Authentisierungstag und Advisory-gepruefter Stack-Basis ist vorbereitet, wird aber bewusst erst nach drittem IMU und erster UART-Grundkette aktiviert.
Naechster Schritt: dritten IMU anbinden, LED-Debugging und Buzzer pruefen, Roboterarm aufbauen und UART-Inbetriebnahme zum Arduino.
Zusaetzliche Post-v1-Ausbaurichtungen werden gesammelt unter `future/FUTURE_WORK.md`.
