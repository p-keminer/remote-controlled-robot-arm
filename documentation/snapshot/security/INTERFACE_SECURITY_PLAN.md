# Interface Security Plan

Dieses Dokument beschreibt die Sicherheitsregeln fuer lokale Service-, Debug- und Datenschnittstellen.

## Relevante Schnittstellen

- ESP32-Programmier- und Debugzugriff
- I2C-Verbindung zwischen Receiver (Master) und Arduino (Slave 0x42)
- lokale Bench- und Serviceanschluesse

## Grundregeln

- Servicepfade sind als lokale Entwicklungswege zu behandeln, nicht als dauerhafte Betriebsinterfaces
- Debug- oder Servicezugriffe muessen klar von der eigentlichen Bewegungssteuerung getrennt sein
- Default-Konfigurationen duerfen keine weit offenen Betriebsmodi voraussetzen

## I2C-Schnittstelle (Receiver → Arduino)

- akzeptiert nur das dokumentierte Bewegungsframe (Frame V1, 11 Bytes, Slave 0x42)
- keine allgemeine Befehlsschnittstelle fuer unspezifizierte Kommandos
- kein implizites Umschalten in riskante Betriebszustaende durch freie Texteingaben
- ISR-minimales Design: Wire.onReceive() kopiert nur Rohbytes, Validierung in loop()

## Debug- und Servicezugang

- nur lokal und bewusst genutzt
- Dokumentation soll festhalten, welche Servicezugriffe im Entwicklungsbetrieb benoetigt werden
- fuer den spaeteren Zielbetrieb sollen ungenutzte Debugpfade minimiert oder deaktiviert werden
