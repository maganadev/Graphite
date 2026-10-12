#!/bin/bash
set -euo pipefail

# Build unit tests on Linux using the UnitTesting/CMakePresets.json LinuxUBSANRelease preset.
cd "$(dirname "$0")/UnitTesting"

pwd
rm -f ./build/LinuxUBSANRelease/GraphiteClangCheckTests
cmake --preset LinuxUBSANRelease
cmake --build build/LinuxUBSANRelease/ --target format
cmake --build build/LinuxUBSANRelease/

export UBSAN_OPTIONS=suppressions=../SanitizerSuppresions/UBSAN.supp && ./build/LinuxUBSANRelease/GraphiteClangCheckTests
