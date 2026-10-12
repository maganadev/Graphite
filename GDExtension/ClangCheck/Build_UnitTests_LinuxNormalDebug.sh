#!/bin/bash
set -euo pipefail

# Build unit tests on Linux using the UnitTesting/CMakePresets.json LinuxNormalDebug preset.
cd "$(dirname "$0")/UnitTesting"

pwd
rm -f ./build/LinuxNormalDebug/GraphiteClangCheckTests
cmake --preset LinuxNormalDebug
cmake --build build/LinuxNormalDebug/ --target format
cmake --build build/LinuxNormalDebug/

./build/LinuxNormalDebug/GraphiteClangCheckTests
