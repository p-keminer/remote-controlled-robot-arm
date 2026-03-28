#!/bin/bash
# sync_wsl_to_windows.sh
# Synchronisiert NUR den firmware/-Ordner von WSL nach Windows.
# Wird zum Kompilieren und Flashen per Arduino IDE/CLI benoetigt.

set -euo pipefail
SKRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
KONFIG="$SKRIPT_DIR/sync_config.local.sh"

if [ ! -f "$KONFIG" ]; then
    echo "FEHLER: $KONFIG nicht gefunden."
    exit 1
fi

source "$KONFIG"

if [ -z "${WSL_REPO:-}" ] || [ -z "${WIN_REPO:-}" ]; then
    echo "FEHLER: WSL_REPO und WIN_REPO muessen in sync_config.local.sh gesetzt sein."
    exit 1
fi

EXTRA=""
[ "${1:-}" = "--dry-run" ] && EXTRA="--dry-run" && echo "=== TROCKENLAUF ==="

mkdir -p "${WIN_REPO}firmware/"

rsync -av --delete $EXTRA \
    --exclude=".pio/" \
    --exclude="build/" \
    --exclude="__pycache__/" \
    "${WSL_REPO}firmware/" "${WIN_REPO}firmware/"

echo "Firmware-Sync abgeschlossen: WSL -> Windows (nur firmware/)"
