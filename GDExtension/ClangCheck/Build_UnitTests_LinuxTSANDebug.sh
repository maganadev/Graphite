#!/bin/bash
set -euo pipefail

# Build unit tests on Linux using the UnitTesting/CMakePresets.json LinuxTSANDebug preset.
cd "$(dirname "$0")/UnitTesting"

pwd
rm -f ./build/LinuxTSANDebug/GraphiteClangCheckTests
cmake --preset LinuxTSANDebug
cmake --build build/LinuxTSANDebug/ --target format
cmake --build build/LinuxTSANDebug/

export TSAN_OPTIONS=suppressions=../SanitizerSuppresions/TSAN.supp && ./build/LinuxTSANDebug/GraphiteClangCheckTests
