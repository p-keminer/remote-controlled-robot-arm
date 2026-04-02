# Interface Security Plan

Dieses Dokument beschreibt die Sicherheitsregeln fuer lokale Service-, Debug- und Datenschnittstellen.

## Relevante Schnittstellen

- ESP32-Programmier- und Debugzugriff
- serielle Verbindung zwischen Receiver und Arduino
- lokale Bench- und Serviceanschluesse

## Grundregeln

- Servicepfade sind als lokale Entwicklungswege zu behandeln, nicht als dauerhafte Betriebsinterfaces
- Debug- oder Servicezugriffe muessen klar von der eigentlichen Bewegungssteuerung getrennt sein
- Default-Konfigurationen duerfen keine weit offenen Betriebsmodi voraussetzen

## UART-Schnittstelle

- akzeptiert nur das dokumentierte Bewegungsframe
- keine allgemeine Befehlsschnittstelle fuer unspezifizierte Kommandos
- kein implizites Umschalten in riskante Betriebszustaende durch freie Texteingaben

## Debug- und Servicezugang

- nur lokal und bewusst genutzt
- Dokumentation soll festhalten, welche Servicezugriffe im Entwicklungsbetrieb benoetigt werden
- fuer den spaeteren Zielbetrieb sollen ungenutzte Debugpfade minimiert oder deaktiviert werden
