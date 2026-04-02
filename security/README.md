# security

## Zweck

Dieser Ordner enthaelt die Security-Dokumentation des Projekts.
Hier werden Angriffsmodell, Kommunikationshaertung, Schnittstellenregeln und Provisioning-Grundsaetze dokumentiert.

## Aktueller Stand

Die Security-Basis wird in dieser Ausbaustufe erstmals als eigener Projektbereich aufgebaut.
Der Schwerpunkt liegt auf lokaler ESP-NOW- und I2C-Sicherheit, nicht auf WLAN- oder Cloud-Infrastruktur.
Bekannte ESP-NOW-Schwachstellen, Advisory-Lage und frueh mitzudenkende Haertungsmassnahmen werden jetzt als eigener Dokumentationsstrang nachgezogen.

## Inhalt

- `THREAT_MODEL.md` fuer Assets, Angreifer und Missbrauchspfade
- `COMMUNICATION_SECURITY.md` fuer ESP-NOW- und I2C-Sicherheitsregeln
- `INTERFACE_SECURITY_PLAN.md` fuer Debug-, Service- und serielle Schnittstellen
- `PROVISIONING_AND_DEBUG_RULES.md` fuer Secret-Umgang, Pairing und Servicepraxis
- `local/` fuer nicht-repotaugliche lokale Secret-, Peer- und Geraetedaten

## Regeln

- `Security` ist von `Safety` getrennt zu behandeln
- v1 wird auf `ESP-NOW` ohne WLAN ausgelegt
- echte Secrets, produktive IDs oder sicherheitskritische Echtwerte gehoeren nicht ins Repository
- lokale Sensitivdaten gehoeren nur in `security/local/` und muessen durch `.gitignore` blockiert sein
- sicherheitsrelevante Entscheidungen muessen in Root-Dokumente und betroffene Teilbereiche gespiegelt werden

## Schnittstellen/Abhaengigkeiten

- richtet sich nach `../SECURITY_FRAMEWORK.md`
- arbeitet eng mit `../COMMUNICATION_FRAMEWORK.md`, `../SAFETY_FRAMEWORK.md` und `../tests/` zusammen
