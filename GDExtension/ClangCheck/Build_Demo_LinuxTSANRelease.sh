#!/bin/bash
set -euo pipefail

# Build the demo application on Linux using the Demo/CMakePresets.json LinuxTSANRelease preset.
cd "$(dirname "$0")/Demo"

pwd
rm -f ./build/LinuxTSANRelease/GraphiteClangCheck
cmake --preset LinuxTSANRelease
cmake --build build/LinuxTSANRelease/ --target format
cmake --build build/LinuxTSANRelease/

export TSAN_OPTIONS=suppressions=../SanitizerSuppresions/TSAN.supp && ./build/LinuxTSANRelease/GraphiteClangCheck

