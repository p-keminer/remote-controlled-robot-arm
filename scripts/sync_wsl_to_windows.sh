#!/bin/bash
# sync_wsl_to_windows.sh
# Synchronisiert das komplette Repo von WSL nach Windows.
# Ausfuehren aus WSL heraus.
#
# Konfiguration: scripts/sync_config.local.sh mit WSL_REPO und WIN_REPO befuellen.
# Vorlage: scripts/sync_config.local.sh.template

set -euo pipefail
SKRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
KONFIG="$SKRIPT_DIR/sync_config.local.sh"

if [ ! -f "$KONFIG" ]; then
    echo "FEHLER: $KONFIG nicht gefunden."
    echo "Bitte sync_config.local.sh.template als sync_config.local.sh kopieren und anpassen."
    exit 1
fi

source "$KONFIG"

if [ -z "${WSL_REPO:-}" ] || [ -z "${WIN_REPO:-}" ]; then
    echo "FEHLER: WSL_REPO und WIN_REPO muessen in sync_config.local.sh gesetzt sein."
    exit 1
fi

EXTRA=""
[ "${1:-}" = "--dry-run" ] && EXTRA="--dry-run" && echo "=== TROCKENLAUF ==="

rsync -av --delete $EXTRA \
    --exclude='.git/' \
    --exclude='.pio/' \
    --exclude='.vscode/' \
    --exclude='.theia/' \
    --exclude='__pycache__/' \
    --exclude='node_modules/' \
    --exclude='build/' \
    --exclude='.cache/' \
    --exclude='*.local.sh' \
    --exclude='*.local.h' \
    --exclude='*.local.txt' \
    --exclude='*.local.md' \
    --exclude='security/local/' \
    "$WSL_REPO" "$WIN_REPO"

echo "Sync abgeschlossen: WSL -> Windows"
