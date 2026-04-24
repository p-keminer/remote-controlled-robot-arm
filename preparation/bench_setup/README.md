# preparation/bench_setup

## Zweck

Dieser Ordner beschreibt die geordnete Labor- und Prototypumgebung des Projekts.

## Aktueller Stand

Der reine offene Breadboard-Stand ist historisch.
Aktiv ist jetzt ein Prototyp mit fertigem Armaufbau, Controller auf Lochraster-/Perfboard und verifiziertem Debugpfad zu Dashboard und ROS.

Historische Bench-Erkenntnisse bleiben wichtig fuer Teiltests, aber der Arbeitsstand ist heute:

- Controller mit drei IMUs und Potentiometer als Greifer-Eingabe
- Receiver->Arduino-I2C aktiv
- Bridge->Pi->Dashboard/ROS verifiziert

## Regeln

- Bench- und Prototypzustand sauber trennen.
- Historische Testverkabelung nicht mit dem aktuellen Perfboard-Stand verwechseln.
- Messungen und neue Beobachtungen gehoeren in dated Nachweise unter `docs/measurement_logs/`.

## Schnittstellen/Abhaengigkeiten

- `../../hardware/bringup/README.md`
- `../../hardware/electronics/`
- `../../tests/`
