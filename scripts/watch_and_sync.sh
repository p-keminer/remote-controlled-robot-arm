#!/bin/bash
# watch_and_sync.sh
# Beobachtet firmware/ und synct .ino und .h Dateien bei Aenderung nach Windows
#
# Konfiguration: Kopiere scripts/sync_config.local.sh.template
#                nach scripts/sync_config.local.sh und trage deine Pfade ein.
#
# Starten:   bash scripts/watch_and_sync.sh
# Stoppen:   Ctrl+C

SKRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
KONFIG="$SKRIPT_DIR/sync_config.local.sh"

if [ ! -f "$KONFIG" ]; then
    echo "FEHLER: $KONFIG nicht gefunden."
    echo "Bitte sync_config.local.sh.template als sync_config.local.sh kopieren und anpassen."
    exit 1
fi

# shellcheck source=sync_config.local.sh.template
source "$KONFIG"

BEOBACHTET="$QUELLE"

sync_ausfuehren() {
    rsync -a --delete \
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
    echo "[$(date '+%H:%M:%S')] Sync: $ZIEL"
}

echo "Watcher gestartet. Beobachte: $BEOBACHTET"
echo "Ziel: $ZIEL"
echo "Stoppen mit Ctrl+C"

sync_ausfuehren

while true; do
    inotifywait -r -e modify,create,delete,move \
        --include '.*\.(ino|h)$' \
        "$BEOBACHTET" 2>/dev/null
    sync_ausfuehren
done
