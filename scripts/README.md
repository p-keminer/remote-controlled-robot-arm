# scripts

## Zweck

Dieser Ordner enthaelt Pflege- und Hilfsskripte fuer Dokumentation, Sync und lokale Projektautomation.

## Aktueller Stand

Der wichtigste aktive Helfer ist `update_docs.sh`.
Das Skript sammelt die verteilte Quelldokumentation des Repos nach `documentation/` und spiegelt jetzt nicht mehr nur Markdown, sondern auch repo-gepflegte Doku-Medien wie README-GIFs und Dashboard-Screenshots.

## Inhalt

- `update_docs.sh` erzeugt den Snapshot unter `documentation/`
- `sync_wsl_to_windows.sh` synchronisiert das Repo von WSL nach Windows
- `sync_windows_to_wsl.sh` synchronisiert das Repo von Windows nach WSL
- `secret_scan.sh` prueft auf typische Secrets und lokale Echtwerte
- `install_hooks.sh` installiert die Git-Hooks fuer den Secret-Scan
- `sync_config.local.sh.template` ist die Vorlage fuer lokale Sync-Pfade

## Regeln

- Skripte enthalten keine harten Nutzer- oder Systempfade.
- Lokale Pfade und Zugangsdaten gehoeren ausschliesslich in gitignorierte `*.local.*`-Dateien.
- Skripte bilden Projektzustand ab, erzeugen aber keine stillschweigende Produktivfreigabe.
- Der Doku-Snapshot bleibt generiert; gepflegt werden die Quelldokumente im eigentlichen Repo.

## Doku-Snapshot

`update_docs.sh` sammelt aktuell:

- alle projektgepflegten Markdown-Dateien ausser generierten und Vendor-Bereichen
- README-GIFs unter `docs/photos/readme/`
- Dashboard-Screenshots unter `dashboard/web/screenshots/`

Nicht gespiegelt werden weiter:

- `official_downloads/raw/`
- `official_downloads/extracted/`
- `security/local/`
- generierte Build- und Snapshot-Bereiche

## Schnittstellen/Abhaengigkeiten

- arbeitet auf der Quelldokumentation des Repositories
- schreibt ausschliesslich in den generierten Ordner `documentation/`
- beachtet die Secret- und Local-File-Regeln aus `../security/` und `../GLOBAL_RULES.md`
