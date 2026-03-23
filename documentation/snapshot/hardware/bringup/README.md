# hardware/bringup

## Zweck

Dieser Ordner beschreibt Vorpruefungen, sichere Erstinbetriebnahme und die Reihenfolge erster Hardwaretests.

## Aktueller Stand

Die Bringup-Dokumentation ist vorbereitet.
Die reale Inbetriebnahme folgt spaeter entlang der hier beschriebenen Struktur.
Vor Bewegungs- oder Umbautests muss zuerst ein dokumentierter Stock-Baseline-Test des originalen Adeept-Arms erfolgen.
Der offizielle Herstellerdownload fordert dabei qualifizierte `18650`-Zellen und warnt davor, den Arm im Betrieb mit der Hand zu zwingen.
Als aktuelle Beschaffungsbasis sind `4x Molicel INR-18650-M35A` ohne Loetfahne plus `1x XTAR VC4SL` dokumentiert; reale Batteriefach-Passung und Bewegungsfreigabe bleiben aber noch offen.

## Inhalt

- Vorpruefungen vor erstem Einschalten
- Strom- und Segmentfreigaben fuer schrittweise Tests
- Bezug zu Safety-, Security- und Bench-Regeln
- Trennung zwischen vorsichtigem Stock-Test und spaeterem modifiziertem Projektbetrieb

## Regeln

- keine Erstinbetriebnahme ohne dokumentierte Vorbedingungen
- `Micro USB` nicht automatisch als belastbarer Mehrservo-Strompfad annehmen; Stromentscheidung vor realen Bewegungsversuchen dokumentieren
- jede kritische Beobachtung spaeter in `docs/measurement_logs/` nachziehen
- den Arm im aktiven Betrieb nicht per Hand zwangsbewegen; manuelle Korrekturen nur in sicherem, dokumentiertem Zustand

## Schnittstellen/Abhaengigkeiten

- verweist auf `../../preparation/bench_setup/`, `../../tests/safety/`, `../../tests/security/` und `../../SAFETY_FRAMEWORK.md`
- verweist fuer reale Strompfade und Stock-vs-Projekt-Trennung zusaetzlich auf `../electronics/POWER_SUPPLY_CONCEPT.md` und `../ADEEPT_ARM_PRODUCT_BASELINE.md`

## Recherchequellen

- [Adeept 5-DOF Robotic Arm Kit Compatible with Arduino IDE](https://www.adeept.com/adeept-5-dof-robotic-arm-kit-compatible-with-arduino-ide-programmable-diy-coding-stem-educational-5-axis-robot-arm-with-oled-display-processing-code_p0360.html) fuer die Herstellerangabe, dass `2x18650` benoetigt werden koennen und `Micro USB` alternativ moeglich ist.
- [Adeept Forum: Adeept 5-DOF Arduino ROBOT ARM](https://www.adeept.com/forum/thread-1009-1-1.html) fuer die offizielle Support-Aussage, dass `Micro USB` fuer Mehrservo-Betrieb zu wenig Strom liefern kann und `18650`-Batterien empfohlen werden.
- [Read me first.pdf](../../official_downloads/extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Read%20me%20first.pdf) fuer die Herstellerwarnung, den Arm nicht im Betrieb mit der Hand zu zwingen, und fuer den Hinweis auf qualifizierte `18650`-Zellen.
- [Adeept V4 Package Review](../../official_downloads/ADEEPT_V4_PACKAGE_REVIEW.md) fuer die projektrelevante Zusammenfassung des offiziellen Herstellerstands.
