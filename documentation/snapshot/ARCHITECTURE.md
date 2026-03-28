# Architektur

Dieses Dokument beschreibt die feste Systemstruktur des Roboterarm-Projekts.

## Zielbild

Das System uebersetzt Koerperbewegungen in sichere Servo-Zielwinkel.
Die Gesamtarchitektur wird in klar getrennte Ebenen zerlegt:

- Vorbereitung und Toolchain-Basis
- Wearable-Sensorik und Vorverarbeitung
- lokaler Funkkanal ueber `ESP-NOW`
- Receiver-Bridge (Steuerpfad)
- Servoausfuehrung
- Security- und Safety-Vorgaben
- Kalibrierung, Hardware- und Nachweisfluss
- Entwicklungs-Dashboard (Debug-Beobachtungsebene, nicht Teil des Steuerpfads)

## Systemgrenzen

### Vorbereitungsebene

- beschreibt Toolchain, Bench, Hardware-Readiness und Vorbedingungen
- fuehrt keine Firmware aus, sondern schafft die nachvollziehbare Basis dafuer

### Wearable-Seite

- ein ESP32 liest BNO055-IMUs ueber Multiplexer und den Flex-Sensor ueber ADC
- die Controller-Firmware berechnet daraus relative oder direkt abgeleitete Gelenkzielwerte
- die Wearable-Seite enthaelt keine direkte Servo-PWM

### Funkstrecke

- die drahtlose Strecke transportiert kompakte Zielwerte und Integritaetsinformationen
- `ESP-NOW` ist der vorgesehene lokale Funkpfad fuer v1
- WLAN, Internet und Cloud sind nicht Teil der v1-Architektur
- Bewegungsframes sollen zusaetzlich Session-, Replay- und applikationsseitige Authentisierung beruecksichtigen

### Roboterseite

- ein zweiter ESP32 (Receiver) empfaengt Funkdaten, validiert sie und leitet sie ueber UART weiter
- das Adeept-Arduino-Board setzt Zielwerte in reale Servoauslenkungen um
- mechanische Grenzwerte und Rampen greifen spaetestens auf der Arduino-Ebene

### Entwicklungs-Dashboard-Ebene (optional, nicht Teil des Steuerpfads)

- ein dritter ESP32 (Bridge) empfaengt die gleichen ESP-NOW Pakete wie der Receiver als zweiter Peer
- die Bridge leitet die Daten per WiFi und MQTT an einen Mosquitto-Broker auf dem Raspberry Pi weiter
- das Dashboard zeigt Live-Sensordaten, Statistiken und eine 3D-Simulation im Browser
- ein MQTT MCP Server erlaubt Claude direkten Zugriff auf die Live-Sensordaten fuer KI-gestuetztes Debugging
- die Bridge ist rein beobachtend und kann keine Steuerbefehle in den Steuerpfad einschleusen
- ein Ausfall der Bridge oder des Dashboards hat keinen Einfluss auf den Steuerpfad (Receiver → Arduino)

## Datenfluss

### Steuerpfad (sicherheitskritisch)

1. IMUs und Flex-Sensor liefern Rohdaten an den Controller-ESP32.
2. Der Controller berechnet segmentbezogene oder relative Winkel.
3. Das Ergebnis wird in ein `ESP-NOW`-Frame (ImuPaket v3) mit Integritaets- und Frischeannahmen ueberfuehrt.
4. Der Receiver-ESP32 validiert das Frame und erstellt daraus ein serielles Uebergabeformat.
5. Der Arduino setzt die Zielwerte unter Beruecksichtigung von Limits, Rampen und Neutralverhalten um.

### Debug-Pfad (Entwicklungswerkzeug, nicht sicherheitskritisch)

3b. Das gleiche ESP-NOW-Frame geht parallel als zweiter Peer an die Bridge.
4b. Die Bridge validiert das Paket und publiziert es als JSON per MQTT auf dem Pi.
5b. Dashboard, MCP Server und api.php lesen die MQTT-Topics mit.

## Persistente Artefakte

- Vorbereitungs- und Toolchainwissen liegt unter `preparation/`
- Security-Regeln und Bedrohungsmodell liegen unter `security/`
- Referenzposen, Limits und Mappingannahmen werden unter `calibration/` dokumentiert
- Hardware-, Aufbau- und Bringupwissen liegt unter `hardware/`
- Messungen, Fotos und Integrationsnachweise werden unter `docs/` abgelegt
- generierte Sammlungen liegen nur in `documentation/`

## Architekturregeln

- Vorbereitung, Hardwarebeschreibung und Laufzeitlogik werden nicht vermischt.
- Kalibrierungs- oder Security-Wissen darf nicht nur als versteckte Konstante in Firmware verbleiben.
- Eine neue Schnittstelle gilt erst dann als eingefuehrt, wenn der passende Framework-Text mitgezogen wurde.
