#!/bin/bash
set -euo pipefail

# Build the demo application on Linux using the Demo/CMakePresets.json LinuxUBSANRelease preset.
cd "$(dirname "$0")/Demo"

pwd
rm -f ./build/LinuxUBSANRelease/GraphiteClangCheck
cmake --preset LinuxUBSANRelease
cmake --build build/LinuxUBSANRelease/ --target format
cmake --build build/LinuxUBSANRelease/

export UBSAN_OPTIONS=suppressions=../SanitizerSuppresions/UBSAN.supp && ./build/LinuxUBSANRelease/GraphiteClangCheck

