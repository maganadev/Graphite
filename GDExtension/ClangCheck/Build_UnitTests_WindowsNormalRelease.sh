#!/bin/bash
set -euo pipefail

# Build unit tests on Linux using the UnitTesting/CMakePresets.json WindowsNormalRelease preset.
cd "$(dirname "$0")/UnitTesting"

pwd
rm -f ./build/WindowsNormalRelease/GraphiteClangCheckTests
cmake --preset WindowsNormalRelease
cmake --build build/WindowsNormalRelease/ --target format
cmake --build build/WindowsNormalRelease/

./build/WindowsNormalRelease/GraphiteClangCheckTests
