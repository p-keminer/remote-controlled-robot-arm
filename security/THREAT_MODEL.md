# Threat Model

Dieses Dokument beschreibt die wichtigsten Assets, Angreifer und Missbrauchspfade fuer v1.

## Schutzgueter

- autorisierte Bewegungssteuerung des Roboterarms
- Integritaet der ESP-NOW-Kommunikation
- Integritaet der UART-Uebergabe an den Arduino
- Kalibrierungs- und Grenzwerte
- lokale Service- und Debugzugaenge

## Angreiferklassen

- nahe stehender Funkteilnehmer mit eigenem ESP32 oder kompatibler Hardware
- lokaler Angreifer mit physischem Zugriff auf Debug- oder Serviceports
- unbeabsichtigte Fehlbedienung durch unsaubere Entwicklungs- oder Serviceprozesse
- Entwicklerfehler wie harte Secrets im Repo oder unkontrollierte Beispielkonfigurationen

## Vertrauensgrenzen

- Controller-ESP32 zu Funkkanal
- Funkkanal zu Receiver-ESP32
- Receiver-ESP32 zu UART-Frame
- lokaler Rechner oder Servicezugang zu Debug- und Provisioning-Pfaden

## Wesentliche Missbrauchspfade

- ein fremder Peer sendet Bewegungsframes an den Receiver
- ein aufgezeichneter gueltiger Frame wird erneut abgespielt
- ein unvollstaendiger oder manipulierte UART-Frame beeinflusst die Servoausgabe
- Debugschnittstellen bleiben im Zielbetrieb offen oder unkontrolliert zugreifbar
- Kalibrier- oder Grenzwerte werden unkontrolliert ueberschrieben

## V1-Gegenmassnahmen

- feste Peer-Zuordnung statt offener Teilnehmerannahme
- dokumentiertes Konzept fuer Integritaet, Versionskennung und Frische eines Frames
- definierte Fehlerreaktion fuer unbekannte, veraltete oder ungueltige Daten
- keine produktiven Secrets, MAC-Listen oder Pairingdaten im Repo
- physische Debug- und Servicepfade nur als lokale Entwicklungswege behandeln
