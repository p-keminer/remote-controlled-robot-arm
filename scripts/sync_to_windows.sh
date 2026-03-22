#!/bin/bash
# sync_to_windows.sh
# Kopiert nur die Sketch-Dateien nach Windows (OneDrive)
# Benoetigt: .ino und .h Dateien fuer Arduino IDE

QUELLE="/home/p-keminer/projects/embedded/robotic-arm/firmware/"
ZIEL="/mnt/c/Users/pkemi/OneDrive/Desktop/Projekte/5doof_robotarm/firmware/"

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
