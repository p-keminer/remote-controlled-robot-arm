# tests/safety

## Zweck

Dieser Ordner ist fuer sicherheitsrelevante Test- und Freigabeszenarien vorgesehen.

## Aktueller Stand

Die Struktur ist vorbereitet.
Konkrete Grenz-, Timeout- und Stoppszenarien folgen mit der Inbetriebnahme.

## Inhalt

- Grenzwertpruefungen fuer Servolimits
- Timeout- und Watchdog-Szenarien
- Neutralpositions- und Stopverhalten
- spaetere Freigabe- oder Totmann-Tests

## Regeln

- sicherheitsrelevante Faelle so dokumentieren, dass Vorbedingung, Ausloeser und erwartete Reaktion klar sind
- fehlgeschlagene oder riskante Beobachtungen duerfen nicht nur in lockeren Notizen verbleiben

## Schnittstellen/Abhaengigkeiten

- richtet sich nach `SAFETY_FRAMEWORK.md`
- nutzt Grenzwerte aus `calibration/servo_limits/` und Messdaten aus `docs/measurement_logs/`
