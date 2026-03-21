# Security Framework

Dieses Dokument definiert die verbindlichen Sicherheitsgrundlagen des Projekts.
Es beschreibt, wie das System gegen unautorisierte Steuerung, unsichere Schnittstellen, unbeabsichtigte Freigaben und unsauberen Secret-Umgang abgesichert werden soll.

## Sicherheitszielbild

Die erste Projektversion wird als lokal betriebenes Embedded-System behandelt:

- `ESP-NOW` ist der einzige vorgesehene Funkpfad zwischen den ESP32
- `UART` ist die interne Uebergabe zwischen Receiver und Arduino
- WLAN, Internetanbindung, Cloud, Web-API und Fernwartung sind in v1 ausdruecklich nicht Teil des Zielsystems

## Trennung zu Safety

- `Security` behandelt Angriffe, Missbrauch, unautorisierte Steuerung, Provisioning, Debugzugriff und Secret-Umgang.
- `Safety` behandelt physische Bewegungssicherheit wie Gelenklimits, Neutralposition, Rampen und Watchdog-Reaktion.
- Aenderungen mit Auswirkung auf beide Bereiche muessen in `SECURITY_FRAMEWORK.md` und `SAFETY_FRAMEWORK.md` gespiegelt werden.

## Schutzobjekte

- Bewegungsautoritaet ueber den Roboterarm
- Peer-Zuordnung zwischen Controller-ESP32 und Receiver-ESP32
- serielle Uebergabe an die Arduino-Servoebene
- Kalibrierungsdaten, Grenzwerte und Referenzzustaende
- spaetere Secret- oder Pairing-Daten
- Debug- und Provisioning-Zugaenge

## V1-Entscheidungen

- keine WLAN-Infrastruktur und keine Webschnittstellen dokumentieren
- keine produktiven Secrets im Repository ablegen
- keine offene Broadcast-Steuerung als Standardpfad zulassen
- keine unkontrollierte Debug- oder Provisioning-Schnittstelle fuer den Zielbetrieb vorsehen
- feste, dokumentierte Peer-Beziehungen statt dynamischer automatischer Kopplung anstreben

## Kommunikationssicherheit

- Funkkommunikation wird als dedizierter lokaler Steuerkanal behandelt
- unbekannte oder nicht freigegebene Peers duerfen keine Bewegungsdaten einspeisen
- Frames muessen Integritaets-, Versions- und Frischeannahmen unterliegen
- Replay- oder Duplikatverhalten muss als eigener Sicherheitsfall beschrieben werden

## Schnittstellensicherheit

- UART darf nur klar definierte, validierbare Bewegungsframes akzeptieren
- Debug- und Wartungsschnittstellen muessen als lokale Servicepfade beschrieben werden
- unsichere Generalschnittstellen wie Kommando-Shells oder unstrukturierte Fernkonfiguration sind fuer v1 nicht vorgesehen

## Secret- und Provisioning-Regeln

- echte Produktionswerte, Peer-Listen oder Sicherheitsschluessel kommen nicht ins Repo
- Beispielwerte muessen klar als Platzhalter markiert werden
- Provisioning wird als kontrollierter manueller Prozess fuer lokale Entwicklungsgeraete behandelt

## Nachweis und Pflege

- Detailregeln liegen unter `security/`
- technische Auswirkungen muessen in Kommunikations-, Architektur- und Testdokumente gespiegelt werden
- nach jedem groesseren Security-relevanten Schritt muessen Dokumente aktualisiert und `bash ./scripts/update_docs.sh` ausgefuehrt werden
