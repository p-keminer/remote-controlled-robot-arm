# Wearable Final Wear System

Dieses Dokument beschreibt den spaeteren stabilen Betriebstyp fuer das koerpergetragene Sensorsystem.

## Zielbild

Der spaetere Betriebstyp soll:

- schneller und reproduzierbarer anlegbar sein als V1
- weniger verrutschen
- die Sensororientierung klar erzwingen
- auch bei laengerem Betrieb angenehmer tragbar sein
- den Austausch von Sensorboards weiter erlauben

## Leitidee fuer den Endtyp

Der Endtyp soll **kein loses Gurtbasteln** mehr sein, sondern ein modulares Soft-Wearable:

- **textile oder neoprenartige Manschette**
- **formstabile IMU-Aufnahme / Sensordock**
- **Anti-Rutsch-Innenlage**
- **definierte Ausrichtung**
- **saubere Kabelfuehrung**
- **separates Elektronikmodul** fuer Controller/Akku statt Elektronik direkt auf jedem IMU-Segment

## Aufbau des Endsystems

### Oberarm-Modul

- Manschette fuer den unteren Oberarm
- fester Dock an der Außenseite des Arms
- Markierung fuer oben/unten und vorne/hinten
- optionale zweite Sicherung gegen Rotieren

### Unterarm-Modul

- Manschette fuer den unteren Unterarm
- Dock auf der Rueckseite/Oberseite
- Kabelfuehrung vom Handgelenk weg
- keine harte Kante direkt in Beuge- oder Druckzonen

### Hand-/Wrist-Modul

- Handruecken- oder Wrist-Modul mit kleiner Dock-Struktur
- spaeter moeglich als Teil eines leichten fingerlosen Handschuhs
- fuer Greifbewegungen weniger stoerend als ein frei verrutschender Gurt

## Endtyp-Skizze

```text
Endtyp pro Segment

 [ IMU-Modul ]
      |
 [ austauschbarer Dock ]
      |
 [ textile Manschette mit Grip-Innenseite ]
      |
 [ Markierung fuer Ausrichtung ]
      |
   Armsegment
```

```text
Systemidee

   Oberarm-Manschette ----\
                           \ 
   Unterarm-Manschette ----- >  separates Elektronikmodul
                           /   (z. B. Schulter, Brust oder Guertel)
   Wrist-/Hand-Modul ------/
```

## Warum ein textilbasierter Endtyp?

- Eine aktuelle Uebersicht zeigt, dass Upper-Limb-Wearables sehr unterschiedliche Trageformen nutzen, von Gurten bis zu textilen Loesungen; es gibt keinen einzigen klaren Standard, aber die Wearability ist ein zentrales Thema. Quelle: [Tracking Upper Limb Motion via Wearable Solutions](https://pmc.ncbi.nlm.nih.gov/articles/PMC11704657/).
- Textile oder weich getragene Upper-Limb-Systeme sind auch in anderen Wearable-/Reha-Kontexten attraktiv, weil sie leichter und koerpernah integrierbar sind. Als Konzeptinspiration fuer den spaeteren Betriebstyp nutzen wir deshalb zusaetzlich: [A Wearable Soft Fabric Sleeve for Upper Limb Augmentation](https://pmc.ncbi.nlm.nih.gov/articles/PMC8620533/).

## Anforderungen an den Endtyp

- der Sensor darf sich nicht frei auf dem Textil verdrehen
- die Lage muss beim erneuten Anlegen moeglichst gleich wiedergefunden werden
- Druckpunkte muessen klein bleiben
- Funk- oder Hochfrequenzkomponenten sollen nicht direkt neben der IMU liegen
- Sensordock und Sensorboard sollen austauschbar bleiben

## Einfluss der Elektronik auf die Mechanik

- Laut Adafruit ist der BNO055 empfindlich gegen bestimmte I2C-Konstellationen und gegen hochfrequente Leitungen in Sensornähe. Quelle: [Adafruit BNO055 Guide](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-bno055-absolute-orientation-sensor.pdf).
- Deshalb wird fuer den Endtyp empfohlen:
  - IMU klein und lokal am Segment halten
  - Controller, Funk und groessere Stromversorgung nicht direkt auf die IMU-Manschette setzen
  - Sensorhalterung als austauschbaren Dock bauen

## Geplanter Uebergang von V1 zu Endtyp

1. V1 mit Klett und starrer Platte testen
2. funktionierende Positionen mit Fotos und Mounting-Record festhalten
3. aus diesen Positionen eine indexierbare Manschette ableiten
4. Sensorplatte in einen formstabilen Dock ueberfuehren
5. Elektronik und Kabelfuehrung vom IMU-Dock entkoppeln

## Forschungseinfluss auf die Platzierung

- Fuer die eigentliche Segmentlage folgen wir weiterhin der Placement-Empfehlung: Oberarm außen und unten, Unterarm Rueckseite/Oberseite und unten. Quelle: [Grip et al., 2021](https://pubmed.ncbi.nlm.nih.gov/34167702/).
- Ein weiteres Wearable-Beispiel mit arm- und unterarmgetragenen IMUs und klarer Koordinatensystem-Definition bestaerkt den Ansatz, die Segmentausrichtung mechanisch erkennbar zu machen: [Custom IMU-Based Wearable System](https://pubmed.ncbi.nlm.nih.gov/34640961/).

## Recherchequellen

- [Tracking Upper Limb Motion via Wearable Solutions](https://pmc.ncbi.nlm.nih.gov/articles/PMC11704657/) fuer den Vergleich verschiedener Upper-Limb-Wearable-Formen und die Ableitung eines tragbaren Endsystems.
- [A Wearable Soft Fabric Sleeve for Upper Limb Augmentation](https://pmc.ncbi.nlm.nih.gov/articles/PMC8620533/) als Inspiration fuer textile, koerpernahe und laenger tragbare Soft-Wearable-Konzepte.
- [Grip et al., 2021, PubMed 34167702](https://pubmed.ncbi.nlm.nih.gov/34167702/) fuer die weiterverwendete Segmentplatzierung an Oberarm und Unterarm.
- [Custom IMU-Based Wearable System](https://pubmed.ncbi.nlm.nih.gov/34640961/) fuer den Fokus auf klar erkennbare Segmentausrichtung und reproduzierbares Koordinatensystem.
- [Adafruit BNO055 Guide](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-bno055-absolute-orientation-sensor.pdf) fuer den Einfluss von Elektroniknaehe, I2C-Randbedingungen und Austauschbarkeit des Sensordocks.
