# Wearable Prototype V1

Dieses Dokument beschreibt den ersten praktischen Trage-Prototyp fuer Oberarm-, Unterarm- und Hand-/Wrist-IMU.

## Ziel des Prototyps

Der Prototyp soll schnell baubar sein und vier Fragen beantworten:

- sitzt die IMU an der Zielposition stabil genug?
- bleibt die Bewegungsfreiheit alltagstauglich?
- laesst sich das System wiederholt aehnlich anlegen?
- verrutscht das System bei einfachen Testbewegungen nicht zu stark?

## Prototyp-Idee

Der erste Prototyp basiert auf:

- **breitem Klettgurt oder Sportarmband**
- **rutschhemmender Zwischenlage** aus Neopren, Gummi-Print oder Sporttextil
- **kleiner starrer Sensorplatte**
- **optionalem Querband gegen Verdrehen**

## Aufbau je Segment

### Oberarm

- breiter Gurt um den unteren Oberarm
- Sensorplatte an der Außenseite des Oberarms
- Position: knapp ueber dem Ellenbogen
- optional zweites kurzes Querband, damit die Platte nicht nach vorne rollt

### Unterarm

- breiter Gurt um den unteren Unterarm
- Sensorplatte auf der Rueckseite/Oberseite des Unterarms
- Position: nahe Handgelenk, aber nicht direkt auf dem Gelenk
- Kabelzug muss vom Handgelenk weggefuehrt werden

### Hand/Wrist

- erster Startpunkt: Gurt direkt **ueber dem Handgelenk**
- Sensorplatte oben auf dem Gurt oder auf einem kurzen Handrueckenfluegel
- nicht direkt dort platzieren, wo der Handruecken beim Greifen stark geknickt wird

## Prototyp-Skizze

```text
Seitlicher Querschnitt eines Gurts

   [ IMU-Modul ]
        |
   [ starre Platte ]
        |
   [ Anti-Rutsch-Pad ]
        |
   [ breiter Klettgurt ]
        |
      Armsegment
```

```text
Oberarm-Prototyp V1

   Außenseite des Arms
        [ IMU ]
         ===          <- starre Platte
   ----------------   <- breiter Hauptgurt
      Oberarm
        |
      Ellenbogen
```

## Materialidee fuer V1

- 40-50 mm breites Klettband oder Sportarmband
- Neopren oder aehnliches weiches Antirutschmaterial
- kleine Tragerplatte aus leichtem Kunststoff, Acryl oder 3D-Druck
- Kabelentlastung mit Textilschlaufe oder weichem Clip

## Warum Klett/V1-Straps?

- In vielen Upper-Limb-Wearable-Studien werden **Velcro-Straps, Straps, Armbands oder Wristbands** fuer die Segmentbefestigung verwendet. Quelle: [Tracking Upper Limb Motion via Wearable Solutions](https://pmc.ncbi.nlm.nih.gov/articles/PMC11704657/).
- Auch bei orthotischen Upper-Limb-Systemen werden mehrpunktige Straps genutzt, um Arm- und Unterarmsegmente stabil zu sichern. Diese Quelle ist keine IMU-Placement-Studie, hat aber das Prototyp-Denken fuer die Gurtlogik beeinflusst: [Dynamic Assist Elbow Orthosis](https://pmc.ncbi.nlm.nih.gov/articles/PMC6255727/).

## Testkriterien fuer V1

- 10 bis 20 Wiederholungen Armheben, Beugen und Drehen ohne deutlich sichtbares Verrutschen
- keine schmerzhafte Einschraenkung in Schulter oder Ellenbogen
- Anlegen in unter 2 Minuten pro Segment erreichbar
- nach erneutem Anlegen aehnliche Foto- und Sensorlage reproduzierbar

## Was V1 bewusst noch nicht loest

- kein schoenes Dauertragesystem
- keine integrierte Textilhülle
- keine finale Kabelfuehrung
- keine finale Sweat-, Langzeit- oder Alltagstauglichkeit

## Dokumentationspflicht fuer V1

- reale Anlegeversuche spaeter mit `docs/templates/SENSOR_MOUNTING_RECORD_TEMPLATE.md` dokumentieren
- Fotos in `docs/photos/`
- Auffaelligkeiten zu Verrutschen, Druckstellen oder Kabelzug in `docs/measurement_logs/` oder `docs/session_notes/`

## Recherchequellen

- [Tracking Upper Limb Motion via Wearable Solutions](https://pmc.ncbi.nlm.nih.gov/articles/PMC11704657/) fuer die Nutzung von Velcro-Straps, Armbands und aehnlichen Befestigungen in Upper-Limb-Wearables.
- [Dynamic Assist Elbow Orthosis](https://pmc.ncbi.nlm.nih.gov/articles/PMC6255727/) als Konzeptinspiration fuer mehrpunktige Strap- und Segmentfixierung, auch wenn die Quelle nicht speziell eine IMU-Placement-Studie ist.
- [Grip et al., 2021, PubMed 34167702](https://pubmed.ncbi.nlm.nih.gov/34167702/) fuer die weiterhin zugrunde gelegte Zielzone an Oberarm und Unterarm, auf die der Prototyp V1 mechanisch hinarbeitet.
