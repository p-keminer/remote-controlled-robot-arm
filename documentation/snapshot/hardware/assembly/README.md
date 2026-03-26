# hardware/assembly

## Zweck

Dieser Ordner dokumentiert den Aufbau des Roboterarms als nachvollziehbaren Ablauf.

## Aktueller Stand

Der Adeept 5-DOF Roboterarm ist mechanisch im Stock-Zustand aufgebaut (bestaetigt 2026-03-24, Fotos unter docs/photos/).
Ein Stock-Baseline-Test (Servo90, Unpacking_test_code) steht noch aus und wird vor jeglichen Umbauten als Pflichtschritt behandelt.
Der offizielle Herstellerstand legt dabei nahe, vor dem eigentlichen Aufbau zuerst den `Servo90`-Pfad und danach den `Unpacking_test_code` als Stock-Referenz mitzudenken.

## Inhalt

- Aufbauphasen fuer mechanische und elektrische Teilmontage
- Verweise auf Inventar, Verkabelung und Sensormontage
- Nachweispunkte fuer den spaeteren Selbstaufbau

## Regeln

- keine impliziten Aufbauannahmen ohne Nachweis
- zuerst den originalen Adeept-Aufbau und die Stock-Funktion dokumentieren, danach erst Umbauten am Projektpfad
- reale Abweichungen spaeter in `docs/` und `hardware/verkabelung/` festhalten
- Aufbau und erste Tests sollen sich am offiziellen V4.0-Tutorialpfad orientieren, bevor projektspezifische Abweichungen eingefuehrt werden

## Schnittstellen/Abhaengigkeiten

- arbeitet mit `../mechanik/`, `../verkabelung/`, `../../preparation/robot_arm_readiness/` und `../../docs/templates/`

## Recherchequellen

- [Adeept Video: Adeept 5-DOF Robotic Arm Assembly Tutorial | ADA031 Tutorial](https://www.adeept.com/video/detail-164.html) fuer den offiziellen Assembly-Bezug des Stock-Systems.
- [Adeept Learn: New Robotic Arm Kit for Arduino - V4.0](https://www.adeept.com/learn/detail-64.html) fuer die offizielle Tutorial- und Ressourcenbasis des Kits.
- [Adeept V4 Package Review](../../official_downloads/ADEEPT_V4_PACKAGE_REVIEW.md) fuer die Einordnung von `Servo90`, `Unpacking_test_code` und dem importierten Herstellerpfad.
