#!/bin/bash
set -euo pipefail

# Run clang-format on the given files. Files whose formatting is already
# correct are left untouched so that modification times (and therefore
# incremental builds) are preserved.

if [ "$#" -eq 0 ]; then
    echo "Usage: $0 <file> [file...]" >&2
    exit 1
fi

if ! command -v clang-format >/dev/null 2>&1; then
    echo "error: clang-format was not found in PATH" >&2
    exit 1
fi

changed=0
for file in "$@"; do
    if [ ! -f "$file" ]; then
        echo "error: file not found: $file" >&2
        exit 1
    fi

    if clang-format --dry-run --Werror "$file" >/dev/null 2>&1; then
        continue
    fi

    clang-format -i "$file"
    changed=$((changed + 1))
    echo "Formatted: $file"
done

if [ "$changed" -gt 0 ]; then
    echo "Formatted $changed file(s)."
else
    echo "All files are already formatted; no changes made."
fi