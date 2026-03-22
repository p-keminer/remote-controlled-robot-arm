#!/bin/bash
# sync_to_windows.sh
# Kopiert nur die Sketch-Dateien nach Windows (OneDrive)
# Benoetigt: .ino und .h Dateien fuer Arduino IDE
#
# Konfiguration: Kopiere scripts/sync_config.local.sh.template
#                nach scripts/sync_config.local.sh und trage deine Pfade ein.

SKRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
KONFIG="$SKRIPT_DIR/sync_config.local.sh"

if [ ! -f "$KONFIG" ]; then
    echo "FEHLER: $KONFIG nicht gefunden."
    echo "Bitte sync_config.local.sh.template als sync_config.local.sh kopieren und anpassen."
    exit 1
fi

# shellcheck source=sync_config.local.sh.template
source "$KONFIG"

rsync -av --delete \
    --exclude='.pio/' \
    --exclude='.vscode/' \
    --exclude='src/' \
    --exclude='lib/' \
    --exclude='include/' \
    --include='*/' \
    --include='*.ino' \
    --include='*.h' \
    --exclude='*' \
    "$QUELLE" "$ZIEL"

echo "Sync abgeschlossen: $ZIEL"
