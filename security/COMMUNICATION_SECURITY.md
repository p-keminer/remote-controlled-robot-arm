# Communication Security

Dieses Dokument beschreibt die erste Security-Grundlage fuer die Kommunikationspfade des Projekts.

## V1-Kommunikationsgrenzen

- `ESP-NOW` ist der lokale Funkkanal zwischen Controller und Receiver
- `I2C` ist die lokale Kabelschnittstelle zwischen Receiver (Master, GPIO13/14) und Arduino (Slave 0x42, A4/A5)
- WLAN, Internet und Cloud sind kein Teil der v1-Kommunikation

## ESP-NOW-Grundsaetze

- Steuerdaten duerfen nicht als offener Standard-Broadcast-Pfad geplant werden
- bekannte Peer-Beziehungen sollen vorab definiert und begrenzt werden
- Integritaet darf nicht nur auf “sieht plausibel aus” beruhen
- jedes Bewegungsframe braucht ein dokumentiertes Konzept fuer Protokollversion und Frische

## Bekannte ESP-NOW-Schwachstellen und Grenzen

- Laut Espressif ist `ESP-NOW` nur dann verschluesselt, wenn fuer den Peer eine LMK gesetzt ist; ohne LMK bleibt Unicast unverschluesselt.
- Wenn keine PMK gesetzt wird, wird eine Default-PMK verwendet. Das ist fuer einen steuernden Produktivpfad unzureichend.
- Verschluesselte Broadcast- oder Multicast-Frames werden laut Espressif nicht unterstuetzt. Ein Broadcast-Steuerpfad waere deshalb von Anfang an ein Sicherheitsleck.
- Espressif weist ausserdem darauf hin, dass MAC-seitiger Sendeerfolg nicht garantiert, dass die Anwendung die Nachricht korrekt und einmalig verarbeitet hat. Sequenz- und ACK-Logik bleiben also Pflicht.
- Fuer die optionale Komponente `espressif/esp-now` existieren offizielle Advisories fuer Replay-Angriffe und eine OOB-Schwachstelle in Gruppen-Nachrichten bis `2.5.1`.
- Im offiziellen Replay-Advisory zur optionalen `espressif/esp-now`-Komponente wird selbst die gepatchte Loesung nicht als vollstaendig replay-sicher bewertet; eigene Session- und Replay-Abwehr bleibt also Pflicht.
- Fuer `ESP-IDF` selbst existiert eine offizielle `ESP-NOW`-Advisory wegen Integer Underflow in der Empfangslogik.

## Schon jetzt mitzudenkende Gegenmassnahmen

### Funkpfad und Peer-Modell

- Bewegungsdaten nur als dedizierter Unicast-Pfad, nie als Broadcast oder Gruppen-Nachricht
- feste Peer-Allowlist mit explizit provisionierten MACs
- PMK immer explizit setzen
- pro Geraetepaar eigene LMK vorsehen
- Kanal und Interface bewusst festlegen statt unkontrolliert offen lassen

### Anwendungsrahmen

- jedes Bewegungsframe braucht `session_id`, monotonen `zaehler`, klaren `msg_type` und Status-Flags
- jedes Bewegungsframe braucht zusaetzlich einen applikationsseitigen Authentisierungstag
- alte `session_id` oder ruecklaeufige Zaehler muessen verworfen werden
- Steuerung wird erst nach explizitem `armed`- oder `ready`-Zustand akzeptiert
- der aktuelle Bench-Pfad mit XOR-Pruefsumme und Frische-Check bleibt ausdruecklich ein Zwischenstand und ersetzt diese Security-Baseline nicht
- die I2C-Grundkette Receiver → Arduino ist bench-validiert (bestaetigt 2026-04-02); der Security-Uplift ist der naechste Schritt; ab der naechsten Paketrevision muessen mindestens `protokoll_version`, `flags` und klar erkennbare Erweiterungspunkte fuer spaetere Security-Felder mitgefuehrt werden

### Empfangs- und Parse-Haertung

- Laengen- und Strukturpruefungen vor jeder tieferen Verarbeitung
- Empfangs-Callback nur zum Weiterreichen in eine sichere Queue nutzen, nicht fuer tiefe Logik
- Rate-Limits und Duplikatcache gegen Flooding und Wiederholungen vorsehen
- bei unklarer oder veralteter Stack-Version keinen Bewegungsbetrieb freigeben
- die committed Stack-Zielbasis muss in `../preparation/esp32_environment/README.md` stehen; lokal geflashte Ist-Versionen gehoeren in eine gitignorierte Datei unter `./local/`

## I2C-Grundsaetze

- die I2C-Verbindung (Receiver GPIO13/14 → Arduino A4/A5, Slave 0x42) uebertraegt ausschliesslich klar strukturierte Bewegungsframes
- ungepruefte oder unvollstaendige Frames duerfen nicht in direkte Bewegung uebergehen
- Diagnose oder Debugwerte duerfen die Bewegungssteuerung nicht implizit vermischen

## Fehler- und Missbrauchsfaelle

- unbekannter Peer -> Frame verwerfen
- ungueltige Integritaetspruefung -> Frame verwerfen
- veraltete oder doppelte Frames -> nicht zur Bewegung verwenden
- ungueltige serielle Nutzlast -> nicht an die Servoebene durchreichen
- fehlende LMK / unverschluesselter Steuerpfad -> kein Produktivbetrieb
- bekannte verwundbare `ESP-NOW`- oder `ESP-IDF`-Version -> kein Produktivbetrieb

## Erste Implementierungsleitlinien

- `XOR`, `CRC` oder reine Plausibilitaet gelten nur als Korruptions- oder Debughilfe, nicht als Security
- Applikations-ACK soll an `session_id` und `zaehler` gebunden sein
- Komponenten- und Framework-Versionen muessen vor erstem Realbetrieb dokumentiert und gegen Advisories geprueft sein
- die optionale Nutzung von `espressif/esp-now` ist nur zulaessig, wenn die gepatchte Release-Lage dokumentiert ist

## Dokumentationspflicht

- jede Protokollaenderung muss auch in `../COMMUNICATION_FRAMEWORK.md` gespiegelt werden
- sicherheitsrelevante Testfaelle gehoeren nach `../tests/security/`

## Recherchequellen

- [ESP-IDF ESP-NOW Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html) fuer CCMP, Default-PMK, LMK-Pflicht, Broadcast-Grenzen und Anwendungs-ACK/Sequenznummern.
- [Replay Attacks Vulnerability In ESP-NOW](https://github.com/espressif/esp-now/security/advisories/GHSA-wf6q-c2xr-77xj) fuer die Advisory-Lage der optionalen `espressif/esp-now`-Komponente bei Replay-Angriffen.
- [OOB Vulnerability In ESP-NOW Group Type Message](https://github.com/espressif/esp-now/security/advisories/GHSA-q6f6-4qc5-vhx5) fuer die dokumentierte Gruppen-Nachrichten-Schwachstelle der optionalen `espressif/esp-now`-Komponente.
- [ESP-NOW Integer Underflow Vulnerability Advisory](https://github.com/espressif/esp-idf/security/advisories/GHSA-hqhh-cp47-fv5g) fuer die Verwundbarkeit im `ESP-IDF`-Stack und die Upgrade-Pflicht.
