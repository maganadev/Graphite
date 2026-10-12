#!/bin/bash
set -euo pipefail

# Build unit tests on Linux using the UnitTesting/CMakePresets.json LinuxNormalRelease preset.
cd "$(dirname "$0")/UnitTesting"

pwd
rm -f ./build/LinuxNormalRelease/GraphiteClangCheckTests
cmake --preset LinuxNormalRelease
cmake --build build/LinuxNormalRelease/ --target format
cmake --build build/LinuxNormalRelease/

./build/LinuxNormalRelease/GraphiteClangCheckTests
