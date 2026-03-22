#!/bin/bash
# watch_and_sync.sh
# Beobachtet firmware/ und synct .ino und .h Dateien bei Aenderung nach Windows
#
# Starten:   bash scripts/watch_and_sync.sh
# Stoppen:   Ctrl+C

QUELLE="/home/p-keminer/projects/embedded/robotic-arm/firmware/"
ZIEL="/mnt/c/Users/pkemi/OneDrive/Desktop/Projekte/5doof_robotarm/firmware/"
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
