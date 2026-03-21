# scripts

## Zweck

Dieser Ordner enthaelt Hilfsskripte fuer Pflege, Automatisierung und spaetere Tooling-Aufgaben.

## Aktueller Stand

Aktuell ist vor allem das WSL-faehige Dokumentationsupdate vorgesehen.
Weitere Build-, Flash- oder Testhelfer koennen spaeter hier sauber einsortiert werden.

## Inhalt

- `update_docs.sh` sammelt die verteilte Projektdokumentation in `documentation/` und schliesst generierte oder externe Bereiche aus

## Regeln

- Skripte duerfen keine Fachlogik des Roboters enthalten
- Pflegehelfer sollen moeglichst idempotent und klar nachvollziehbar sein
- Skripte sollen deutliche Fehlerausgaben und einen reproduzierbaren Ablauf liefern

## Schnittstellen/Abhaengigkeiten

- arbeitet auf der Markdown-Quelldokumentation des Repositories
- schreibt nur in den generierten Ordner `documentation/`
