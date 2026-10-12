#!/bin/bash
set -euo pipefail

# Build unit tests on Linux using the UnitTesting/CMakePresets.json LinuxUBSANDebug preset.
cd "$(dirname "$0")/UnitTesting"

pwd
rm -f ./build/LinuxUBSANDebug/GraphiteClangCheckTests
cmake --preset LinuxUBSANDebug
cmake --build build/LinuxUBSANDebug/ --target format
cmake --build build/LinuxUBSANDebug/

export UBSAN_OPTIONS=suppressions=../SanitizerSuppresions/UBSAN.supp && ./build/LinuxUBSANDebug/GraphiteClangCheckTests
