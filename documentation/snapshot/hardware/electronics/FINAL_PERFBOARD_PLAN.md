# Final Perfboard Plan

Dieses Dokument beschreibt den spaeteren dauerbetriebsnahen Elektronikaufbau nach der offenen Bench-Phase.
Der Zielpfad ist zunaechst `Lochraster / Perfboard`, nicht sofort eine eigene PCB.

## Ziel

- die offenen Breadboard-Aufbauten in robuste Trage- und Robotikmodule ueberfuehren
- Debugbarkeit erhalten, aber mechanische Stabilitaet deutlich steigern
- Gehaeuse, Zugentlastung und Steckkonzept frueh mitdenken

## Grundsatz

`Bench = Breadboard`, `dauerbetriebsnah = Lochraster / Perfboard`.

Eine spaetere eigene PCB bleibt offen, aber die erste robuste Ausbaustufe soll bereits ohne lose Steckverkabelung auskommen.

## Sender-Endstufe

### Elektroniktraeger

- `ESP32-S3 DevKitC-1 N16R8`
- kleine Lochraster- oder Adapterplatine fuer feste Signalfuehrung
- feste Anschluesse fuer IMU-Leitungen, Flex-Sensor und Status-LEDs
- spaeter optionale Steckverbinder mit Verriegelung oder zumindest klarer Zugentlastung

### Gehaeuseidee

Das Sender-Gehaeuse soll:

- kompakt und tragbar sein
- DevKit plus Traegerplatine aufnehmen
- USB-Zugang fuer Firmware und Diagnose frei lassen
- Reset- und Boot-Zugang ohne Vollzerlegung erlauben
- Kabel seitlich oder nach unten mit Zugentlastung herausfuehren
- Oesen, Schlitze oder Befestigungspunkte fuer Gurt, Pouch oder Brust-/Schultertraeger besitzen
- die Antennenzone des Moduls frei halten

### Trageidee

- nicht direkt auf kleinen IMU-Baendern
- bevorzugt an Schultergurt, Brustmodul, Guertel oder separatem Oberarm-Elektroniktraeger
- mit kurzer, sauber gefuehrter Leitung zu den IMU-Modulen

## Receiver-Endstufe

### Elektroniktraeger

- `ESP32-S3 DevKitC-1 N16R8`
- feste Lochraster- oder Adapterplatine fuer UART, LED-Ausgaenge und Buzzer
- geordnete Leitungswege zu Arduino, Stromversorgung und Diagnosepunkten

### Gehaeuseidee

Das Receiver-Gehaeuse soll:

- am Robotersockel oder an einer festen Tragrahmenplatte montierbar sein
- von aussen sichtbare Statusanzeigen besitzen
- den Buzzer akustisch nach aussen fuehren
- USB- und Wartungszugang behalten
- Signalverkabelung und energiereiche Leitungen sauber trennen
- nicht die Funkleistung durch schlechte Antennenlage oder metallische Abschattung verschlechtern

## 3D-Druck als bevorzugter Gehaeusepfad

Fuer beide Seiten ist `3D-Druck` der bevorzugte erste Gehaeusepfad, weil:

- Abstaende, Oeffnungen und Haltepunkte leicht iterierbar sind
- DevKit, Lochraster und Steckverbinder an den realen Bestand angepasst werden koennen
- spaetere Aenderungen an Gurtaufnahme, LED-Fenstern oder Buzzeroeffnung schnell umsetzbar bleiben

Falls kein 3D-Druck direkt verfuegbar ist, sind als Zwischenpfad auch `Pouch + Tragerplatte` oder einfache Gehaeuse mit mechanischer Nacharbeit akzeptabel.

## Einfache Endstufen-Skizze

```text
Sender-Endstufe

 [ IMU-Kabel ] ---> [ Gehaeuse ]
                     | ESP32-S3 |
                     | Perfboard|
                     | LEDs     |
                     +----USB---+
                          |
                    Gurt / Pouch / Schulter

Receiver-Endstufe

 [ ESP-NOW ]
      |
 [ Gehaeuse ] ---> UART ---> Arduino
 | ESP32-S3 |
 | Perfboard|
 | LEDs     |
 | Buzzer   |
 +----USB---+
      |
  Sockel / Chassis
```

## Verbindliche Regeln fuer die Endstufe

- kein Breadboard im Zielaufbau
- kein einziges Statussignal darf nur intern und unsichtbar bleiben
- Debugzugang fuer Flashing und Logs darf nicht verbaut werden
- die Funkantenne braucht Freiraum und darf nicht unbedacht hinter Metall oder enges Gehaeusematerial gesetzt werden
- die eigentlichen IMU-Docks bleiben austauschbar und nicht mit schwerer Haupt-Elektronik verschmolzen

## Akzeptanzkriterien fuer den Wechsel in den Endpfad

- bevorzugter Pinplan ist bench-validiert
- Stecker, Leitungswege und Trageort sind dokumentiert
- Gehaeuseoeffnungen fuer USB, LEDs und Buzzer sind bekannt
- die gewaehlte Anordnung verschlechtert die Funkstrecke nicht offensichtlich

## Recherchequellen

- [ESP32-S3-DevKitC-1 User Guide v1.1](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.1.html) fuer Boardabgriff, Grundlayout und den spaeteren Zugang zu Boot, Reset und USB.
- [ESP32-S3 PCB Layout Design Guidelines](https://docs.espressif.com/projects/esp-hardware-design-guidelines/en/latest/esp32s3/pcb-layout-design.html) fuer Antennenfreiraum, Platzierung auf Basisboards und den Einfluss des Gehaeuses auf RF.
- [ESP Hardware Design FAQ](https://docs.espressif.com/projects/esp-faq/en/latest/hardware-related/hardware-design.html) fuer allgemeine Hinweise zu Modulplatzierung, Antennenbereich und Gehaeuseeinfluss.
- [Adafruit BNO055 Guide](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-bno055-absolute-orientation-sensor.pdf) fuer den Hinweis, Sensorik und stoerende Elektronik moeglichst zu entkoppeln.
