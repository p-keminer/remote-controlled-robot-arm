# Communication Security

Dieses Dokument beschreibt die erste Security-Grundlage fuer die Kommunikationspfade des Projekts.

## V1-Kommunikationsgrenzen

- `ESP-NOW` ist der lokale Funkkanal zwischen Controller und Receiver
- `UART` ist die lokale Kabelschnittstelle zwischen Receiver und Arduino
- WLAN, Internet und Cloud sind kein Teil der v1-Kommunikation

## ESP-NOW-Grundsaetze

- Steuerdaten duerfen nicht als offener Standard-Broadcast-Pfad geplant werden
- bekannte Peer-Beziehungen sollen vorab definiert und begrenzt werden
- Integritaet darf nicht nur auf “sieht plausibel aus” beruhen
- jedes Bewegungsframe braucht ein dokumentiertes Konzept fuer Protokollversion und Frische

## UART-Grundsaetze

- die serielle Verbindung uebertraegt ausschliesslich klar strukturierte Bewegungsframes
- ungepruefte oder unvollstaendige Frames duerfen nicht in direkte Bewegung uebergehen
- Diagnose oder Debugwerte duerfen die Bewegungssteuerung nicht implizit vermischen

## Fehler- und Missbrauchsfaelle

- unbekannter Peer -> Frame verwerfen
- ungueltige Integritaetspruefung -> Frame verwerfen
- veraltete oder doppelte Frames -> nicht zur Bewegung verwenden
- ungueltige serielle Nutzlast -> nicht an die Servoebene durchreichen

## Dokumentationspflicht

- jede Protokollaenderung muss auch in `../COMMUNICATION_FRAMEWORK.md` gespiegelt werden
- sicherheitsrelevante Testfaelle gehoeren nach `../tests/security/`
