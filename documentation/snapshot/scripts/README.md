# scripts

## Zweck

Dieser Ordner enthaelt Hilfsskripte fuer Pflege, Automatisierung und spaetere Tooling-Aufgaben.

## WICHTIG fuer KI-Agenten: Security-Richtlinien gelten hier

**Sobald du diese README liest, sind folgende Dokumente sofort zu lesen, bevor du Skripte aenderst oder neue erstellst:**

- `../GLOBAL_RULES.md` — Abschnitt "Security und lokale Daten"
- `../SECURITY_FRAMEWORK.md` — Abschnitt "Secret- und Provisioning-Regeln"
- `../CLAUDE.md` — Abschnitt "Kein Hartcodieren" und "Vor jedem Push"

**Verbindliche Regeln fuer diesen Ordner:**

- Skripte duerfen **keine** absoluten nutzer- oder systemspezifischen Pfade enthalten (`/home/username/`, `/mnt/c/Users/username/`, Windows-Pfade)
- Konfigurierbare Pfade gehoeren ausschliesslich in gitignorierte lokale Konfigurationsdateien (`*.local.sh`)
- Committed werden nur Templates (`*.local.sh.template`) mit Platzhaltern — niemals die befuellten lokalen Dateien
- `.gitignore` deckt `scripts/*.local.sh` ab — diese Dateien duerfen niemals eingecheckt werden
- Vor jedem Push sind Skripte aktiv auf absolute Pfade zu pruefen

## Aktueller Stand

Aktuell ist vor allem das WSL-faehige Dokumentationsupdate und der Windows-Sync vorgesehen.
Weitere Build-, Flash- oder Testhelfer koennen spaeter hier sauber einsortiert werden.

## Inhalt

- `update_docs.sh` — sammelt die verteilte Projektdokumentation in `documentation/` und schliesst generierte, lokale oder externe Bereiche wie `official_downloads/raw/` und `official_downloads/extracted/` aus
- `sync_wsl_to_windows.sh` — synchronisiert das komplette Repo von WSL nach Windows per rsync (erfordert `sync_config.local.sh`)
- `sync_windows_to_wsl.sh` — synchronisiert das komplette Repo von Windows nach WSL per rsync (erfordert `sync_config.local.sh`)
- `secret_scan.sh` — scannt 10 Kategorien von Secrets (IPs, MACs, Passwoerter, Pfade, SSIDs, SSH-Daten, Keys, Ports, .local-Dateien); wird automatisch als Pre-Commit/Pre-Push Hook ausgefuehrt
- `install_hooks.sh` — installiert Pre-Commit und Pre-Push Git-Hooks fuer den Secret-Scanner
- `sync_config.local.sh.template` — **Vorlage** fuer lokale Pfad-Konfiguration; als `sync_config.local.sh` kopieren und anpassen

## Setup fuer Sync-Skripte

```bash
cp scripts/sync_config.local.sh.template scripts/sync_config.local.sh
# sync_config.local.sh editieren und eigene Pfade eintragen
# sync_config.local.sh ist gitignoriert und darf nicht eingecheckt werden
```

## Regeln

- Skripte duerfen keine Fachlogik des Roboters enthalten
- Pflegehelfer sollen moeglichst idempotent und klar nachvollziehbar sein
- Skripte sollen deutliche Fehlerausgaben und einen reproduzierbaren Ablauf liefern

## Schnittstellen/Abhaengigkeiten

- arbeitet auf der Markdown-Quelldokumentation des Repositories
- spiegelt nur projektgepflegte Markdown-Dokumente, nicht den kompletten entpackten Vendor-Stand
- schreibt nur in den generierten Ordner `documentation/`
