#!/bin/bash
set -euo pipefail

# Build unit tests on Linux using the UnitTesting/CMakePresets.json LinuxTSANRelease preset.
cd "$(dirname "$0")/UnitTesting"

pwd
rm -f ./build/LinuxTSANRelease/GraphiteClangCheckTests
cmake --preset LinuxTSANRelease
cmake --build build/LinuxTSANRelease/ --target format
cmake --build build/LinuxTSANRelease/

export TSAN_OPTIONS=suppressions=../SanitizerSuppresions/TSAN.supp && ./build/LinuxTSANRelease/GraphiteClangCheckTests
