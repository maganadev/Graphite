#!/bin/bash
set -euo pipefail

# Build the demo application on Linux using the Demo/CMakePresets.json LinuxUBSANDebug preset.
cd "$(dirname "$0")/Demo"

pwd
rm -f ./build/LinuxUBSANDebug/GraphiteClangCheck
cmake --preset LinuxUBSANDebug
cmake --build build/LinuxUBSANDebug/ --target format
cmake --build build/LinuxUBSANDebug/

export UBSAN_OPTIONS=suppressions=../SanitizerSuppresions/UBSAN.supp && ./build/LinuxUBSANDebug/GraphiteClangCheck

