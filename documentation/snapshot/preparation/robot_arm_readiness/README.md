# preparation/robot_arm_readiness

## Zweck

Dieser Ordner dokumentiert, welche Voraussetzungen vor physischem Aufbau und erster Inbetriebnahme erfuellt sein muessen.

## Aktueller Stand

Die Readiness-Kriterien sind angelegt und groesstenteils erreicht.
Toolchain, Controller-Bench, Receiver-Bench und Sensorvalidierung sind abgeschlossen. Der Arm ist mechanisch im Stock-Zustand aufgebaut (bestaetigt 2026-03-24). Noch offen: Stock-Baseline-Test, Inventardokumentation, reale Batteriefach-Passung und erste Servo-Freigabe.
Der offizielle Adeept-V4.0-Download ist ausgewertet; fuer den Stock-Pfad sind damit `Arduino Uno`, CH341-Treiber, `Servo90` und `Unpacking_test_code` als Herstellerbasis sichtbar.
Die aktuelle Beschaffungsbasis fuer den Strompfad ist dokumentiert: `4x Molicel INR-18650-M35A` ohne Loetfahne plus `1x XTAR VC4SL`.

## Inhalt

- Voraussetzungen fuer Materialstand, Dokumentation und Toolchain
- Readiness fuer Bench, Stromversorgung und Sicherheitsvorpruefung
- Freigabepunkte vor erstem Zusammenbau oder erstem Einschalten
- Trennung zwischen bench-validierter Elektronikbasis und noch ausstehendem Arm-Aufbau
- Pflicht zum dokumentierten Stock-Baseline-Test vor Umbauten
- Herstellerpfad fuer Stock-Test und Batterieanforderung

## Regeln

- kein Aufbau und keine Inbetriebnahme ohne dokumentierte Vorbedingungen
- offene Punkte muessen sichtbar bleiben und duerfen nicht nur im Kopf existieren
- reale Stromversorgung fuer Servo-Bewegung vor dem modifizierten Betrieb festlegen
- Stock-Readiness soll vor Umbauten den offiziellen Herstellerpfad nicht ueberspringen

## Schnittstellen/Abhaengigkeiten

- verweist auf `../../hardware/assembly/`, `../../hardware/bringup/`, `../../SAFETY_FRAMEWORK.md` und `../../docs/templates/`

## Recherchequellen

- [Adeept V4 Package Review](../../official_downloads/ADEEPT_V4_PACKAGE_REVIEW.md) fuer den Herstellerpfad `Arduino Uno`, CH341-Treiber, `Servo90` und `Unpacking_test_code`.
