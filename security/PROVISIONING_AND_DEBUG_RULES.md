# Provisioning and Debug Rules

Dieses Dokument beschreibt den kontrollierten Umgang mit Pairing-, Service- und Secret-bezogenen Informationen.

## Grundsaetze

- echte Secret-Werte gehoeren nicht ins Repository
- Beispielwerte muessen als Platzhalter erkennbar sein
- Provisioning wird in v1 als manueller, lokaler und dokumentierter Prozess behandelt
- lokale Echtdateien fuer Schluessel, Peer-Listen und Geraeteidentitaeten gehoeren nur nach `security/local/`
- `.gitignore` muss diese lokalen Dateien blockieren

## Pairing und Identitaeten

- Controller und Receiver werden als feste Geraetepaare betrachtet
- Dokumente duerfen beschreiben, welche Identitaetsdaten benoetigt werden, aber keine produktiven Echtwerte enthalten
- Wechsel oder Neuerstellung von Geraeten muss nachvollziehbar protokolliert werden
- lokale Realwerte werden in getrennten lokalen Dateien gehalten und nicht in die oeffentliche Dokumentation uebernommen

## Kein Hartcodieren in Firmware

- MACs, Peer-Adressen, Schluessel und Geraete-IDs duerfen nicht direkt in `.ino`- oder `.cpp`-Dateien hartcodiert werden.
- Diese Werte gehoeren in gitignorierte lokale Konfigurationsdateien im Firmware-Verzeichnis (z.B. `peer_config.local.h`).
- Committed werden nur Template-Header mit Platzhaltern, niemals Echtwerte.
- Die `.gitignore`-Muster `firmware/**/peer_config.local.*`, `firmware/**/credentials.local.*` und `firmware/**/device_identity.local.*` decken diese Dateien ab.
- Vor dem ersten Commit eines Firmware-Ordners muss geprueft werden, dass keine Echtwerte in getrackte Dateien eingeflossen sind.

## Pre-Push Secret-Pruefung

Vor jedem `git push` ist zwingend zu pruefen:

- Sind Secrets, Schluessel, MACs, IP-Adressen oder Passwoerter in getrackte Dateien eingeflossen?
- Deckt `.gitignore` alle lokalen Konfigurationsdateien ab, die Echtwerte enthalten?
- Enthalten Template-Dateien nur Platzhalter, keine Realwerte?
- Sind alle Dateien unter `security/local/` und `firmware/**/*.local.*` gitignoriert?

Wenn eine dieser Pruefungen negativ ausgeht, darf der Push nicht erfolgen.

## Debugpraxis

- Debug-Logs sollen keine sicherheitskritischen Echtwerte unnoetig verbreiten
- Service- oder Testmodi muessen als solche kenntlich gemacht werden
- fuer spaetere Inbetriebnahmen soll dokumentiert sein, welche Debughilfen aktiv sein duerfen und welche vor Realbetrieb wieder aus sein muessen
- lokale Provisioning- oder Servicehinweise mit sensiblen Daten gehoeren in die ignorierten Dateien unter `security/local/`
