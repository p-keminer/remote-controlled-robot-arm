# Projekt Fortschritt

Dieses Dokument fuehrt den aktuellen Managementstand des Projekts.
Es zeigt, in welcher Phase sich das Projekt befindet, welche Arbeitspakete aktiv sind und welche Punkte als Naechstes folgen.

## Gesamtstatus

- Projektmodus: Dokumentations-, Security-, Vorbereitungs- und Hardware-Readiness-Aufbau
- Aktive Hauptphase: Phase 1 - Dokumentationsfundament
- Parallel ausgebaut: Phase 2 - Security-Grundlage, Phase 3 - Vorbereitung, Phase 4 - Hardware-Readiness
- Entwicklungsrealitaet: Konzept und Struktur stehen, aber weder Toolchain noch realer Arm-Aufbau noch erste Firmwarekette sind praktisch umgesetzt

## Aktuelle Phasenampel

| Phase | Status | Einordnung |
| --- | --- | --- |
| Phase 1 - Dokumentationsfundament | in Arbeit | Basis steht und wird weiter synchronisiert |
| Phase 2 - Security-Grundlage | in Arbeit | eigener Bereich ist angelegt |
| Phase 3 - Vorbereitung und Toolchain | in Arbeit | Dokumentationsstruktur steht, reale Setups fehlen |
| Phase 4 - Hardware-Readiness und Nachweisstruktur | in Arbeit | Vorlagen und Hardwareablauf sind im Aufbau |
| Phase 5 - Architektur und Portabilitaetsfundament | vorbereitet | fachlich beschrieben, aber noch nicht technisch heruntergebrochen |
| Phase 6 - Sensorvalidierung | vorbereitet | wartet auf Toolchain, Hardware und Bench-Bereitschaft |
| Phase 7 - Kalibrierung und Mapping | vorbereitet | haengt von realen Sensordaten ab |
| Phase 8 - Kommunikation, Servoausfuehrung und Safety | vorbereitet | Kommunikations- und Safety-Basis ist dokumentiert |
| Phase 9 - Integration und Hardware-Verstetigung | nicht gestartet | folgt erst nach ersten lauffaehigen Teilketten |

## Bereits erledigt

- [x] Root-Dokumente fuer Architektur, Regeln, Kommunikation, Kalibrierung und Safety angelegt
- [x] `PROJEKT_ABLAUFPLAN.md` und `PROJEKT_FORTSCHRITT.md` als Managementschicht eingefuehrt
- [x] `SECURITY_FRAMEWORK.md` und `security/` als eigener Bereich angelegt
- [x] `preparation/` als eigener Vorbereitungsbereich angelegt
- [x] `hardware/assembly/` und `hardware/bringup/` als neue Hardware-Unterbereiche eingefuehrt
- [x] `docs/templates/` mit Roboterarm-bezogenen Vorlagen eingefuehrt
- [x] `.gitignore` und `security/local/` fuer lokale, nicht-repotaugliche Daten vorbereitet
- [x] `CLAUDE.md` als kompakter KI-Brief eingefuehrt

## Noch offen im aktuellen Schwerpunkt

- [ ] Root-Dokumente, lokale Bereichsdokumente und neue Security-/Preparation-Bereiche vollstaendig gegeneinander synchron halten
- [ ] reale Installations- und Readiness-Schritte fuer Arduino IDE, ESP32-Umgebung und Bench mit echtem Stand fuellen
- [ ] reale Hardware-, Inventar- und Aufbauinformationen spaeter in die vorbereiteten Vorlagen ueberfuehren
- [ ] Security-, Safety- und Kommunikationsfaelle spaeter in konkrete Test- und Messdokumente herunterbrechen

## Naechste sinnvolle Arbeitspakete

1. Toolchain- und Bench-Vorbereitung mit echtem lokalen Stand fuellen
2. Hardware-Inventar und Aufbauzustand ueber die neuen Templates dokumentieren
3. erste Sensor- und Kommunikations-Benchs erst starten, wenn Preparation- und Readiness-Kriterien abgehakt sind

## Leitende Dokumente fuer den aktuellen Stand

- `README.md` als Einstieg
- `CLAUDE.md` als Kurzbrief
- `ROADMAP.md` als globale Phasenplanung
- `PROJEKT_ABLAUFPLAN.md` als Management-Reihenfolge
- `SECURITY_FRAMEWORK.md`, `COMMUNICATION_FRAMEWORK.md`, `SAFETY_FRAMEWORK.md` und `CALIBRATION_FRAMEWORK.md` als Systemrahmen
- `preparation/README.md`, `security/README.md`, `hardware/README.md` und `docs/templates/README.md` als neue Schwerpunktbereiche

## Offene Management-Risiken

- Die Dokumentbasis ist jetzt breiter, aber reale Toolchain- und Hardware-Nachweise fehlen noch.
- Ohne konsequente Nutzung der neuen Vorlagen droht der Hardwareteil wieder hinter dem Softwareteil zurueckzufallen.
- Lokale Secret- oder Pairingdaten muessen strikt aus der normalen Dokumentation herausgehalten werden.

## Pflegepflicht fuer dieses Dokument

- nach jedem groesseren Meilenstein aktualisieren
- bei Phasenwechseln den Status dieser Datei und der `ROADMAP.md` gemeinsam anpassen
- nach jeder relevanten Aenderung `bash ./scripts/update_docs.sh` ausfuehren
