#!/bin/bash
# install_hooks.sh — Installiert Git-Hooks fuer das Projekt
#
# Nutzung: bash scripts/install_hooks.sh
# Muss einmalig nach dem Klonen ausgefuehrt werden.

set -euo pipefail

HOOK_DIR="$(git rev-parse --show-toplevel)/.git/hooks"
SKRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Installiere Git-Hooks..."

# Pre-Commit Hook
cat > "$HOOK_DIR/pre-commit" << 'HOOK'
#!/bin/bash
# Pre-Commit Hook: Secret-Scan auf gestagede Dateien
# Installiert von scripts/install_hooks.sh

REPO_ROOT="$(git rev-parse --show-toplevel)"

echo ""
echo "=== Pre-Commit Secret-Scan ==="
echo ""

if ! bash "$REPO_ROOT/scripts/secret_scan.sh" --staged; then
    echo ""
    echo "COMMIT ABGEBROCHEN: Secrets gefunden!"
    echo "Behebe die Treffer oder nutze 'git commit --no-verify' (NUR wenn du sicher bist)."
    exit 1
fi
HOOK

chmod +x "$HOOK_DIR/pre-commit"
echo "  [OK] pre-commit Hook installiert"

# Pre-Push Hook
cat > "$HOOK_DIR/pre-push" << 'HOOK'
#!/bin/bash
# Pre-Push Hook: Secret-Scan auf alle getrackten Dateien
# Scannt NUR Dateien die wirklich im Git liegen (nicht lokale .local-Dateien)
# Installiert von scripts/install_hooks.sh

REPO_ROOT="$(git rev-parse --show-toplevel)"

echo ""
echo "=== Pre-Push Secret-Scan (getrackte Dateien) ==="
echo ""

if ! bash "$REPO_ROOT/scripts/secret_scan.sh" --tracked; then
    echo ""
    echo "PUSH ABGEBROCHEN: Secrets in getrackten Dateien gefunden!"
    echo "Behebe die Treffer bevor du pushst."
    exit 1
fi
HOOK

chmod +x "$HOOK_DIR/pre-push"
echo "  [OK] pre-push Hook installiert"

echo ""
echo "Fertig! Hooks sind aktiv."
echo "  - Pre-Commit: scannt gestagede Dateien"
echo "  - Pre-Push:   scannt getrackte Dateien (die wirklich ins Repo gehen)"
echo "  - Manuell:    bash scripts/secret_scan.sh (scannt ALLES inkl. lokale Dateien)"
