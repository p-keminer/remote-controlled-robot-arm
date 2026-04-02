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
- ein Angreifer nutzt unverschluesselten Unicast oder Default-PMK-basierte Fehlkonfiguration aus
- ein Angreifer oder Stoerpaket triggert Parser- oder Empfangsfehler in einer verwundbaren `ESP-NOW`-Basis
- ein unvollstaendiger oder manipulierte UART-Frame beeinflusst die Servoausgabe
- Debugschnittstellen bleiben im Zielbetrieb offen oder unkontrolliert zugreifbar
- Kalibrier- oder Grenzwerte werden unkontrolliert ueberschrieben

## V1-Gegenmassnahmen

- feste Peer-Zuordnung statt offener Teilnehmerannahme
- dokumentiertes Konzept fuer Integritaet, Versionskennung, `session_id` und Frische eines Frames
- explizite PMK-/LMK-Konfiguration statt Default-PMK und implizit offenem Unicast
- nur Unicast fuer Bewegungsdaten, keine Broadcast- oder Gruppensteuerung
- Advisory-Check der verwendeten `ESP-IDF`- bzw. optionalen `espressif/esp-now`-Basis
- definierte Fehlerreaktion fuer unbekannte, veraltete oder ungueltige Daten
- keine produktiven Secrets, MAC-Listen oder Pairingdaten im Repo
- physische Debug- und Servicepfade nur als lokale Entwicklungswege behandeln

## Recherchequellen

- [ESP-IDF ESP-NOW Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html) fuer die offizielle Sicherheits- und Peer-Modell-Beschreibung von `ESP-NOW`.
- [Replay Attacks Vulnerability In ESP-NOW](https://github.com/espressif/esp-now/security/advisories/GHSA-wf6q-c2xr-77xj) fuer das Replay-Risiko in der optionalen `espressif/esp-now`-Komponente.
- [OOB Vulnerability In ESP-NOW Group Type Message](https://github.com/espressif/esp-now/security/advisories/GHSA-q6f6-4qc5-vhx5) fuer die dokumentierte Parser-/Speicherkorruptionsgefahr der optionalen `espressif/esp-now`-Komponente.
- [ESP-NOW Integer Underflow Vulnerability Advisory](https://github.com/espressif/esp-idf/security/advisories/GHSA-hqhh-cp47-fv5g) fuer die Verwundbarkeit in der `ESP-IDF`-Empfangslogik.
