#!/usr/bin/env bash

set -euo pipefail

project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
documentation_root="$project_root/documentation"
snapshot_root="$documentation_root/snapshot"

rm -rf "$snapshot_root"
mkdir -p "$snapshot_root"

mapfile -t markdown_files < <(
  find "$project_root" \
    \( -type d \( -name documentation -o -name .git -o -name .pio -o -name build -o -name out -o -path "$project_root/security/local" \) -prune \) -o \
    \( -type f -name '*.md' ! -name '*.local.md' ! -name '*.secret.md' ! -name '*.private.md' -print \) |
    LC_ALL=C sort
)

copied_count=0
index_lines=()

for markdown_file in "${markdown_files[@]}"; do
  relative_path="${markdown_file#"$project_root"/}"
  target_path="$snapshot_root/$relative_path"

  mkdir -p "$(dirname "$target_path")"
  cp "$markdown_file" "$target_path"

  index_lines+=("- [$relative_path](snapshot/$relative_path)")
  copied_count=$((copied_count + 1))
done

timestamp="$(date '+%Y-%m-%d %H:%M:%S %Z')"

cat <<EOF > "$documentation_root/README.md"
# documentation

Dieser Ordner ist ein automatisch erzeugtes Archiv der verteilten Projektdokumentation.
Bitte die Dateien in diesem Ordner nicht manuell pflegen.

## Update-Befehl

\`\`\`bash
bash ./scripts/update_docs.sh
\`\`\`

## Letzte Aktualisierung

$timestamp

## Anzahl gesammelter Dokumente

$copied_count
EOF

{
  echo "# Dokumentationsindex"
  echo
  echo "Erzeugt am: $timestamp"
  echo
  echo "## Gesammelte Dokumente"
  echo
  for index_line in "${index_lines[@]}"; do
    echo "$index_line"
  done
} > "$documentation_root/INDEX.md"

echo "Dokumentation aktualisiert: $copied_count Datei(en) gesammelt."
