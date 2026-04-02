# preparation/bench_setup

## Zweck

Dieser Ordner beschreibt die Vorbereitung des Entwicklungs- und Testarbeitsplatzes.

## Aktueller Stand

Die Bench-Basis ist real genutzt und teilweise validiert.
Controller- und Receiver-Boards wurden fuer BNO055, PCA9548A, Flex-Sensor und `ESP-NOW` bereits am offenen Bench getestet; die I2C-Grundkette (Receiver → Arduino) ist bench-validiert (bestaetigt 2026-04-02); LED-, Buzzer- und Lasttests am realen Roboterarm sind noch offen.

## Inhalt

- Stromversorgungs- und Verkabelungsdisziplin
- Reihenfolge fuer sichere Teiltests
- Trennung von Entwicklungsgeraeten, Testaufbau und spaeterem Zielbetrieb
- Bench-Reihenfolge vom Sensorpfad ueber Funk bis zur I2C-/Servo-Integration

## Regeln

- Bench-Aufbau und Realbetrieb nicht vermischen
- unsaubere Ad-hoc-Verkabelung spaeter in `docs/` und `hardware/verkabelung/` nachvollziehbar machen
- Bench-Ergebnisse muessen als echte Nachweise in Doku, Messwerten oder Testdokumenten wieder auftauchen

## Schnittstellen/Abhaengigkeiten

- verweist auf `../../hardware/bringup/`, `../../tests/` und `../../docs/templates/`
