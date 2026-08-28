# documentation

Dieser Ordner ist ein automatisch erzeugter Snapshot der verteilten Projektdokumentation.
Bitte die Dateien in diesem Ordner nicht manuell pflegen.

## Update-Befehl

```bash
tr -d '\r' < scripts/update_docs.sh \
  | sed 's|^project_root=.*$|project_root="$PWD"|' \
  | TZ=Europe/Berlin bash -s
```

## Enthalten

- Markdown-Quelldokumentation aus dem Repository
- repo-gepflegte Doku-Medien aus `docs/photos/readme/`
- Dashboard-Screenshots aus `dashboard/web/screenshots/`

## Nicht enthalten

- generierte oder lokale Verzeichnisse
- `security/local/`
- `official_downloads/raw/`
- `official_downloads/extracted/`

## Letzte Aktualisierung

2026-08-28 03:41:36 GMT

## Anzahl gesammelter Dateien

- Markdown: 28
- Medien: 7
