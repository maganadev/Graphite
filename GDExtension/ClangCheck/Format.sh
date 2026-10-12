#!/bin/bash
set -euo pipefail

# Run clang-format on all .cpp and .hpp files in the project.
cd "$(dirname "$0")"

mapfile -d '' files < <(
    find . \( -name '*.cpp' -o -name '*.hpp' \) \
        -not -path './Demo/build/*' \
        -not -path './UnitTesting/build/*' \
        -print0
)

FormatSources.sh "${files[@]}"