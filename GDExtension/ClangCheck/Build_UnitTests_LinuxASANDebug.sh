#!/bin/bash
set -euo pipefail

# Build unit tests on Linux using the UnitTesting/CMakePresets.json LinuxASANDebug preset.
cd "$(dirname "$0")/UnitTesting"

pwd
rm -f ./build/LinuxASANDebug/GraphiteClangCheckTests
cmake --preset LinuxASANDebug
cmake --build build/LinuxASANDebug/ --target format
cmake --build build/LinuxASANDebug/

export ASAN_OPTIONS=suppressions=../SanitizerSuppresions/ASAN.supp && export LSAN_OPTIONS=suppressions=../SanitizerSuppresions/LSAN.supp && ./build/LinuxASANDebug/GraphiteClangCheckTests
