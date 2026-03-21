# tests/latency

## Zweck

Dieser Ordner enthaelt Methoden und Nachweise fuer das Zeitverhalten des Gesamtsystems.

## Aktueller Stand

Die Struktur ist vorbereitet.
Messmethoden und reale Latenzergebnisse werden spaeter hier dokumentiert.

## Inhalt

- Messstrategie fuer Controller, Funkstrecke, UART und Servoebene
- Zielwerte und Vergleich gegen reale Messungen
- Hilfsnotizen zu Messaufbau und Werkzeugen

## Regeln

- Messpunkte und Zeitbasis immer explizit angeben
- theoretische Zielwerte und reale Messwerte nicht vermischen

## Schnittstellen/Abhaengigkeiten

- nutzt Nachweise aus `docs/measurement_logs/`
- bezieht Kommunikationspfade aus `COMMUNICATION_FRAMEWORK.md`
