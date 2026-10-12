#!/bin/bash
set -euo pipefail

# Build the demo application on Linux using the Demo/CMakePresets.json LinuxASANRelease preset.
cd "$(dirname "$0")/Demo"

pwd
rm -f ./build/LinuxASANRelease/GraphiteClangCheck
cmake --preset LinuxASANRelease
cmake --build build/LinuxASANRelease/ --target format
cmake --build build/LinuxASANRelease/

export ASAN_OPTIONS=suppressions=../SanitizerSuppresions/ASAN.supp && ./build/LinuxASANRelease/GraphiteClangCheck

