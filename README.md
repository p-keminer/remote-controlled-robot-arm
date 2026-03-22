# IMU Robotic Arm

Dieses Repository ist das dokumentationsgetriebene Grundgeruest fuer einen IMU-gesteuerten 5-DOF-Roboterarm auf Basis eines Adeept-Arms.
Das Projekt wird unter WSL entwickelt und trennt bewusst zwischen Vorbereitung, Security, Hardware, Kalibrierung, Firmware und Tests.

## Projektziel

Gebaut werden soll ein Roboterarm, der Bewegungen des menschlichen Arms ueber mehrere BNO055-IMUs und einen Flex-Sensor in Servo-Zielwinkel uebersetzt.
Die erste Version wird als lokal betriebenes Embedded-System vorbereitet: `ESP-NOW` zwischen den ESP32, `UART` zum Arduino, kein WLAN und keine Cloud-Anbindung in v1.

## Aktueller Fokus

Die aktuelle Phase konzentriert sich auf:

- verbindliche Projektstruktur und Dokumentationsregeln
- Security als eigenen Projektbereich
- Vorbereitung fuer Arduino IDE, ESP32-Umgebung, Bench und Inbetriebnahme
- hardwareseitige Dokumentation fuer Inventar, Aufbauablauf und Bringup
- einen reproduzierbaren Dokumentationssnapshot fuer verteilte Markdown-Dokumente

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
Nicht-repotaugliche lokale Werte wie Schluessel, Peer-Listen oder lokale Identitaetsdaten gehoeren nach `security/local/` und duerfen nicht eingecheckt werden.

## Aktueller Entwicklungsstand

Die Konzeptphase ist beschrieben, die strukturierte Dokumentations- und Prozessbasis ist angelegt.
Aktuell werden Security, Vorbereitung, Hardware-Readiness und Vorlagenbasis ausgebaut, bevor die ersten groesseren Firmware- und Integrationsschritte folgen.
Zusaetzliche Post-v1-Ausbaurichtungen werden gesammelt unter `future/FUTURE_WORK.md`.
