#!/usr/bin/env bash

set -euo pipefail

project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
documentation_root="$project_root/documentation"
snapshot_root="$documentation_root/snapshot"

rm -rf "$snapshot_root"
mkdir -p "$snapshot_root"

is_excluded_path() {
  local path="$1"

  case "$path" in
    "$project_root/documentation"/*|\
    "$project_root/.git"/*|\
    "$project_root/.pio"/*|\
    "$project_root/build"/*|\
    "$project_root/out"/*|\
    "$project_root/security/local"/*|\
    "$project_root/official_downloads/raw"/*|\
    "$project_root/official_downloads/extracted"/*)
      return 0
      ;;
  esac

  return 1
}

mapfile -t markdown_files < <(
  find "$project_root" \
    \( -type d \( -name documentation -o -name .git -o -name .pio -o -name build -o -name out -o -path "$project_root/security/local" -o -path "$project_root/official_downloads/raw" -o -path "$project_root/official_downloads/extracted" \) -prune \) -o \
    \( -type f -name '*.md' ! -name '*.local.md' ! -name '*.secret.md' ! -name '*.private.md' -print \) |
    LC_ALL=C sort
)

copied_markdown_count=0
copied_media_count=0
markdown_index_lines=()
media_index_lines=()

copy_with_structure() {
  local source_path="$1"
  local relative_path="${source_path#"$project_root"/}"
  local target_path="$snapshot_root/$relative_path"

  mkdir -p "$(dirname "$target_path")"
  cp "$source_path" "$target_path"
}

for markdown_file in "${markdown_files[@]}"; do
  copy_with_structure "$markdown_file"
  relative_path="${markdown_file#"$project_root"/}"
  markdown_index_lines+=("- [$relative_path](snapshot/$relative_path)")
  copied_markdown_count=$((copied_markdown_count + 1))
done

media_roots=(
  "$project_root/docs/photos/readme"
  "$project_root/dashboard/web/screenshots"
)

for media_root in "${media_roots[@]}"; do
  [[ -d "$media_root" ]] || continue

  while IFS= read -r media_file; do
    is_excluded_path "$media_file" && continue
    copy_with_structure "$media_file"
    relative_path="${media_file#"$project_root"/}"
    media_index_lines+=("- [$relative_path](snapshot/$relative_path)")
    copied_media_count=$((copied_media_count + 1))
  done < <(
    find "$media_root" -type f \
      \( -iname '*.gif' -o -iname '*.png' -o -iname '*.jpg' -o -iname '*.jpeg' -o -iname '*.webp' \) |
      LC_ALL=C sort
  )
done

timestamp="$(date '+%Y-%m-%d %H:%M:%S %Z')"

cat <<EOF > "$documentation_root/README.md"
# documentation

Dieser Ordner ist ein automatisch erzeugter Snapshot der verteilten Projektdokumentation.
Bitte die Dateien in diesem Ordner nicht manuell pflegen.

## Update-Befehl

\`\`\`bash
bash ./scripts/update_docs.sh
\`\`\`

## Enthalten

- Markdown-Quelldokumentation aus dem Repository
- repo-gepflegte Doku-Medien aus \`docs/photos/readme/\`
- Dashboard-Screenshots aus \`dashboard/web/screenshots/\`

## Nicht enthalten

- generierte oder lokale Verzeichnisse
- \`security/local/\`
- \`official_downloads/raw/\`
- \`official_downloads/extracted/\`

## Letzte Aktualisierung

$timestamp

## Anzahl gesammelter Dateien

- Markdown: $copied_markdown_count
- Medien: $copied_media_count
EOF

{
  echo "# Dokumentationsindex"
  echo
  echo "Erzeugt am: $timestamp"
  echo
  echo "## Markdown-Dokumente"
  echo
  for index_line in "${markdown_index_lines[@]}"; do
    echo "$index_line"
  done
  if (( copied_media_count > 0 )); then
    echo
    echo "## Gespiegelte Medien"
    echo
    for index_line in "${media_index_lines[@]}"; do
      echo "$index_line"
    done
  fi
} > "$documentation_root/INDEX.md"

echo "Dokumentation aktualisiert: $copied_markdown_count Markdown-Datei(en), $copied_media_count Mediendatei(en)."
