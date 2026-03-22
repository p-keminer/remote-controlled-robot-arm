# Wearable Sensor Concept

Dieses Dokument beschreibt das uebergeordnete Tragekonzept fuer die IMUs am Koerper.
Es ist die gemeinsame Grundlage fuer den ersten Prototyp und den spaeteren stabilen Betriebstyp.

## Ziel

Die Sensoren sollen:

- moeglichst reproduzierbar anliegen
- nicht deutlich verrutschen
- Schulter, Ellenbogen und Handgelenk nicht unnoetig blockieren
- spaeter auf andere Sensorboards oder Halterungen uebertragbar bleiben

## Einfache Positionsbegriffe

Die Forschung verwendet oft Begriffe wie `lateral`, `distal` oder `dorsal`.
Fuer dieses Projekt nutzen wir bewusst verstaendlichere Begriffe:

| Fachbegriff | Einfach gesagt |
| --- | --- |
| lateral am Oberarm | an der Außenseite des Oberarms |
| distal am Oberarm | eher unten am Oberarm, knapp ueber dem Ellenbogen |
| dorsal am Unterarm | auf der Rueckseite bzw. Oberseite des Unterarms |
| distal am Unterarm | eher unten am Unterarm, nahe am Handgelenk |
| dorsale Hand-/Wrist-Position | auf dem Handruecken oder direkt oberhalb des Handgelenks |

## Zielpositionen im Koerperbild

### Oberarm-IMU

- Zielzone: **Außenseite des Oberarms, knapp ueber dem Ellenbogen**
- Nicht auf der Schulterspitze
- Nicht mitten auf dem Bizepsbauch
- Zweck: moeglichst stabile Segmenterfassung bei noch guter Bewegungsfreiheit

### Unterarm-IMU

- Zielzone: **Rueckseite/Oberseite des Unterarms, nahe am Handgelenk**
- Nicht in der Ellenbeuge
- Nicht auf der Innenseite des Unterarms
- Zweck: gute Erfassung von Unterarmrotation und Ellenbogenbezug

### Hand-/Wrist-IMU

- Zielzone: **auf dem Handruecken oder direkt oberhalb des Handgelenks**
- Startpunkt fuer v1: eher ueber dem Handgelenk statt mitten auf der Hand
- Zweck: stabile Orientierung bei moeglichst wenig Stoerung durch Greifen und Fingerbewegung

## Einfache Lage-Skizze

```text
Rechter Arm - Außenansicht

   Schulter
      |
      |      [ Oberarm-IMU ]
      |      Außenseite, knapp ueber Ellenbogen
      |
   Ellenbogen
      |
      |      [ Unterarm-IMU ]
      |      Rueckseite/Oberseite, nahe Handgelenk
      |
  Handgelenk
      |
      |      [ Hand-/Wrist-IMU ]
      |      Handruecken oder direkt ueber Handgelenk
      |
     Hand
```

## Mechanische Leitideen

- Die IMU sitzt immer auf einer **kleinen starren Traegerplatte**, nicht lose direkt auf Stoff.
- Die Traegerplatte wird auf ein **weiches, rutschhemmendes Textil- oder Neoprenpad** gesetzt.
- Die eigentliche Fixierung erfolgt ueber **breite Gurte oder Manschetten**, nicht ueber schmale Kabelbinder-Logik.
- IMU und Tragesystem sollen **indexierbar** sein: beim erneuten Anlegen moeglichst dieselbe Position und dieselbe Ausrichtung.
- Funk- oder Controller-Hardware soll nicht direkt auf denselben kleinen IMU-Gurten sitzen.

## Warum diese Zielpositionen?

- Eine Placement-Studie fuer Upper-Limb-IMUs empfiehlt den Oberarmsensor **an der Außenseite des unteren Oberarms** und den Unterarmsensor **auf der Rueckseite des unteren Unterarms**. Diese Empfehlung uebernehmen wir hier in verstaendlicher Sprache. Quelle: [Grip et al., 2021, PubMed 34167702](https://pubmed.ncbi.nlm.nih.gov/34167702/).
- Eine systematische Uebersicht zu Upper-Limb-Wearables zeigt, dass viele Projekte fuer Oberarm und Unterarm **Straps, Velcro-Straps, Armbands oder flexible Trageelemente** verwenden. Quelle: [Tracking Upper Limb Motion via Wearable Solutions](https://pmc.ncbi.nlm.nih.gov/articles/PMC11704657/).
- Ein Beispiel fuer ein eigenes Wearable-Layout mit Arm- und Unterarm-Sensoren zeigt, dass eine klare Segmentzuordnung und ein festes Koordinatensystem zentral sind. Quelle: [Custom IMU-Based Wearable System](https://pubmed.ncbi.nlm.nih.gov/34640961/).

## Technische Randbedingungen fuer dieses Projekt

- Das Tragesystem muss **sensorboard-neutral** bleiben, damit spaeter andere IMUs oder andere Elektronik eingesetzt werden koennen.
- Das ist besonders wichtig, weil Adafruits aktuelle BNO055-Doku auf Probleme mit **ESP32 + I2C** und auch mit **I2C-Multiplexern** hinweist. Zusaetzlich wird auf Empfindlichkeit gegen hochfrequente Leitungen in Sensornähe hingewiesen. Quelle: [Adafruit BNO055 Guide](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-bno055-absolute-orientation-sensor.pdf).
- Daraus folgt fuer die Mechanik: IMU-Halterung und groessere Elektronik moeglichst entkoppeln, Kabel sauber fuehren und den eigentlichen IMU-Dock spaeter leicht austauschbar bauen.

## Uebergang in die naechsten Stufen

- `WEARABLE_PROTOTYPE_V1.md` beschreibt den ersten praktischen, einfach baubaren Strap-Prototyp.
- `WEARABLE_FINAL_WEAR_SYSTEM.md` beschreibt den spaeteren stabilen Betriebstyp mit besserer Wiederholbarkeit und weniger Verrutschen.

## Recherchequellen

- [Grip et al., 2021, PubMed 34167702](https://pubmed.ncbi.nlm.nih.gov/34167702/) fuer die uebernommene Grundempfehlung zur Platzierung am Oberarm und Unterarm.
- [Tracking Upper Limb Motion via Wearable Solutions](https://pmc.ncbi.nlm.nih.gov/articles/PMC11704657/) fuer die Einordnung typischer Strap-, Armband- und Wearable-Befestigungen im Upper-Limb-Bereich.
- [Custom IMU-Based Wearable System](https://pubmed.ncbi.nlm.nih.gov/34640961/) fuer die Bedeutung klarer Segmentzuordnung und definierter Sensorausrichtung.
- [Adafruit BNO055 Guide](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-bno055-absolute-orientation-sensor.pdf) fuer die Randbedingungen zu BNO055, ESP32, I2C und stoerender Elektronik in Sensornähe.
