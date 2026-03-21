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

## Debugpraxis

- Debug-Logs sollen keine sicherheitskritischen Echtwerte unnoetig verbreiten
- Service- oder Testmodi muessen als solche kenntlich gemacht werden
- fuer spaetere Inbetriebnahmen soll dokumentiert sein, welche Debughilfen aktiv sein duerfen und welche vor Realbetrieb wieder aus sein muessen
- lokale Provisioning- oder Servicehinweise mit sensiblen Daten gehoeren in die ignorierten Dateien unter `security/local/`
