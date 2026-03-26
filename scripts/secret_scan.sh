#!/bin/bash
# secret_scan.sh — Scannt getrackte Dateien auf Secrets, IPs, Passwoerter und Pfade
#
# Nutzung:
#   bash scripts/secret_scan.sh          # Scannt alle getrackten Dateien
#   bash scripts/secret_scan.sh --staged  # Scannt nur gestagede Dateien (fuer Pre-Commit)
#
# Exit-Code:
#   0 = sauber
#   1 = Treffer gefunden — Commit/Push NICHT durchfuehren

set -euo pipefail

ROT='\033[0;31m'
GRUEN='\033[0;32m'
GELB='\033[0;33m'
CYAN='\033[0;36m'
FETT='\033[1m'
RESET='\033[0m'

FEHLER=0
MODUS="all"
[ "${1:-}" = "--staged" ] && MODUS="staged"

# Dateien ermitteln — ALLE Dateien im Repo-Verzeichnis (nicht nur getrackte)
if [ "$MODUS" = "staged" ]; then
    DATEIEN=$(git diff --cached --name-only --diff-filter=ACM 2>/dev/null)
else
    # Getrackte UND ungetrackte Dateien (alles ausser .git/)
    DATEIEN=$( (git ls-files 2>/dev/null; git ls-files --others --exclude-standard 2>/dev/null; find . -maxdepth 8 -type f 2>/dev/null | sed 's|^\./||') | sort -u)
fi

# Ausschluesse: Vendor-Code, Git-Internals, dieses Skript selbst, Binaerdateien
AUSSCHLUSS="official_downloads/|\.git/|\.claude/|\.pio/|\.theia/|\.vscode/|build/|node_modules/|__pycache__/|secret_scan\.sh|install_hooks\.sh|\.heic$|\.jpg$|\.png$|\.bin$|\.zip$|\.7z$|\.rar$|\.tar$|\.o$|\.elf$|\.map$|\.d$|\.dblite$"
DATEIEN=$(echo "$DATEIEN" | grep -vE "$AUSSCHLUSS" || true)

if [ -z "$DATEIEN" ]; then
    echo -e "${GRUEN}[OK] Keine Dateien zum Scannen.${RESET}"
    exit 0
fi

ANZAHL_DATEIEN=$(echo "$DATEIEN" | wc -l)

echo ""
echo -e "${FETT}=== Secret-Scan ($MODUS) — $ANZAHL_DATEIEN Dateien ===${RESET}"
echo ""

# ============================================================
# Hilfsfunktion: Treffer pruefen und formatiert ausgeben
# ============================================================
pruefe() {
    local NR="$1"
    local NAME="$2"
    local ERGEBNIS="$3"
    local HART="${4:-1}"  # 1=Fehler, 0=Warnung

    echo -n "[$NR] $NAME... "

    if [ -z "$ERGEBNIS" ]; then
        echo -e "${GRUEN}OK${RESET}"
        return
    fi

    local ANZAHL=$(echo "$ERGEBNIS" | wc -l)

    if [ "$HART" = "1" ]; then
        echo -e "${ROT}$ANZAHL Treffer${RESET}"
        FEHLER=$((FEHLER + 1))
    else
        echo -e "${GELB}$ANZAHL Warnung(en)${RESET}"
    fi

    # Jeden Treffer formatiert ausgeben: Datei, Zeile, Inhalt
    while IFS= read -r zeile; do
        local datei=$(echo "$zeile" | cut -d: -f1)
        local zeilennr=$(echo "$zeile" | cut -d: -f2)
        local inhalt=$(echo "$zeile" | cut -d: -f3-)

        echo -e "  ${CYAN}$datei${RESET}:${FETT}$zeilennr${RESET}"
        echo -e "    $inhalt"
    done <<< "$ERGEBNIS"
    echo ""
}

# ============================================================
# Scans
# ============================================================

# 1. Private IPv4-Adressen
T=$(echo "$DATEIEN" | xargs grep -HnE \
    '(192\.168\.[0-9]{1,3}\.[0-9]{1,3}|10\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}|172\.(1[6-9]|2[0-9]|3[01])\.[0-9]{1,3}\.[0-9]{1,3})' \
    2>/dev/null | grep -vE 'PLATZHALTER|HIER_EINTRAGEN|BEISPIEL|example|placeholder|Template|template\.h|template\.py' || true)
pruefe "1/10" "Private IPs" "$T"

# 2. MAC-Adressen (XX:XX:XX:XX:XX:XX)
T=$(echo "$DATEIEN" | xargs grep -HnEi \
    '([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}' \
    2>/dev/null | grep -vE 'PLATZHALTER|HIER_EINTRAGEN|BEISPIEL|example|placeholder|0xAA.*0xBB|0x11.*0x22|template' || true)
pruefe "2/10" "MAC-Adressen" "$T"

# 3. Hex-MAC-Arrays ({0x80, 0xB5, ...})
T=$(echo "$DATEIEN" | xargs grep -HnE \
    '\{[[:space:]]*0x[0-9A-Fa-f]{2}[[:space:]]*,[[:space:]]*0x[0-9A-Fa-f]{2}[[:space:]]*,[[:space:]]*0x[0-9A-Fa-f]{2}[[:space:]]*,[[:space:]]*0x[0-9A-Fa-f]{2}[[:space:]]*,[[:space:]]*0x[0-9A-Fa-f]{2}[[:space:]]*,[[:space:]]*0x[0-9A-Fa-f]{2}' \
    2>/dev/null | grep -vE 'PLATZHALTER|HIER_EINTRAGEN|BEISPIEL|example|placeholder|0xAA.*0xBB.*0xCC|0x11.*0x22.*0x33|template' || true)
pruefe "3/10" "Hex-MAC-Arrays" "$T"

# 4. Passwoerter und API-Schluessel
T=$(echo "$DATEIEN" | xargs grep -HnEi \
    '(password|passwd|passwort|secret|api.?key|token|credential)[[:space:]]*[=:][[:space:]]*["'"'"'][^"'"'"']{4,}' \
    2>/dev/null | grep -vE 'PLATZHALTER|HIER_EINTRAGEN|BEISPIEL|MEIN_|CHANGE_ME|example|placeholder|template|\.template\.|password_hash|htmlspecialchars|config\.php|auth\.php|brute_force|Recherche' || true)
pruefe "4/10" "Passwoerter/Schluessel" "$T"

# 5. WiFi SSIDs
T=$(echo "$DATEIEN" | xargs grep -HnEi \
    '(ssid|wifi.?name)[[:space:]]*[=:][[:space:]]*["\x27][^"\x27]{2,}' \
    2>/dev/null | grep -vE 'PLATZHALTER|HIER_EINTRAGEN|BEISPIEL|MEIN_|example|placeholder|template|\.template\.' || true)
pruefe "5/10" "WiFi SSIDs" "$T"

# 6. Absolute lokale Pfade
T=$(echo "$DATEIEN" | xargs grep -HnE \
    '(/home/[a-z][-a-z0-9]*/|/mnt/c/Users/|C:\\Users\\|/Users/[a-z])' \
    2>/dev/null | grep -vE 'PLATZHALTER|HIER_EINTRAGEN|BEISPIEL|BENUTZER|example|placeholder|template|\.template\.|/home/username/|/Users/username/|/mnt/c/Users/username/' || true)
pruefe "6/10" "Absolute Pfade" "$T"

# 7. SSH-Verbindungsdaten
T=$(echo "$DATEIEN" | xargs grep -HnE \
    'ssh[[:space:]]+-p[[:space:]]+[0-9]+[[:space:]]+[a-z]+@' \
    2>/dev/null | grep -vE 'PLATZHALTER|HIER_EINTRAGEN|BEISPIEL|example|placeholder|template' || true)
pruefe "7/10" "SSH-Verbindungsdaten" "$T"

# 8. Private Keys
T=$(echo "$DATEIEN" | xargs grep -Hn -- \
    '-----BEGIN.*PRIVATE KEY-----' \
    2>/dev/null || true)
pruefe "8/10" "Private Keys" "$T"

# 9. Hartcodierte Ports (Warnung, kein harter Fehler)
T=$(echo "$DATEIEN" | xargs grep -HnEi \
    '(port|listen)[[:space:]]*[=:][[:space:]]*[0-9]{4,5}' \
    2>/dev/null | grep -vE 'PLATZHALTER|HIER_EINTRAGEN|BEISPIEL|example|placeholder|template|\.template\.|\.md:|ROADMAP|README|1883|8765|3232|115200|default|Standard|CONCEPT' || true)
pruefe "9/10" "Hartcodierte Ports" "$T" "0"

# 10. .local Dateien die getrackt werden
T=$(echo "$DATEIEN" | grep -E '\.local\.(h|py|sh|txt|md|json)$' | while read -r f; do
    echo "$f:1:Datei sollte gitignoriert sein"
done || true)
pruefe "10/10" "Getrackte .local Dateien" "$T"

# ============================================================
# Ergebnis
# ============================================================
echo ""
if [ $FEHLER -gt 0 ]; then
    echo -e "${ROT}${FETT}=== FEHLER: $FEHLER Kategorie(n) mit Treffern ===${RESET}"
    echo -e "${ROT}Commit/Push NICHT durchfuehren!${RESET}"
    echo ""
    echo -e "${ROT}${FETT}##############################################################${RESET}"
    echo -e "${ROT}${FETT}#                                                            #${RESET}"
    echo -e "${ROT}${FETT}#   .gitignore AUF BETROFFENE PFADE PRUEFEN!                 #${RESET}"
    echo -e "${ROT}${FETT}#                                                            #${RESET}"
    echo -e "${ROT}${FETT}##############################################################${RESET}"
    exit 1
else
    echo -e "${GRUEN}${FETT}=== SAUBER: Keine Secrets gefunden ===${RESET}"
    exit 0
fi
