# hardware/bringup

## Zweck

Dieser Ordner beschreibt die sichere Inbetriebnahme und den Umgang mit dem aktuellen Prototyp.

## Aktueller Stand

Der Prototyp laeuft bereits im Entwicklungsbetrieb.
Damit verschiebt sich der Schwerpunkt von "erste Stromzufuhr" zu "kontrollierter Betrieb, Debugging und saubere Freigaben".

Weiterhin offen:

- formale Safety-Freigabe
- Security-Uplift
- weitere Strompfad- und Langzeitnachweise

## Regeln

- Debugerfolg ersetzt keine Produktivfreigabe.
- Bewegungsbetrieb, Strompfad und Serviceeingriffe werden dated dokumentiert.
- Der Prototyp bleibt bis zur weiteren Absicherung ein Entwicklungsaufbau.

## Schnittstellen/Abhaengigkeiten

- `../electronics/POWER_SUPPLY_CONCEPT.md`
- `../../tests/`
- `../../SAFETY_FRAMEWORK.md`
