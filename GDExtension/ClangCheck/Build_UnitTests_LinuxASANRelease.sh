#!/bin/bash
set -euo pipefail

# Build unit tests on Linux using the UnitTesting/CMakePresets.json LinuxASANRelease preset.
cd "$(dirname "$0")/UnitTesting"

pwd
rm -f ./build/LinuxASANRelease/GraphiteClangCheckTests
cmake --preset LinuxASANRelease
cmake --build build/LinuxASANRelease/ --target format
cmake --build build/LinuxASANRelease/

export ASAN_OPTIONS=suppressions=../SanitizerSuppresions/ASAN.supp && ./build/LinuxASANRelease/GraphiteClangCheckTests
