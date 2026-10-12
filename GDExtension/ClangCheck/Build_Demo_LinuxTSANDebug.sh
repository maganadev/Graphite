#!/bin/bash
set -euo pipefail

# Build the demo application on Linux using the Demo/CMakePresets.json LinuxTSANDebug preset.
cd "$(dirname "$0")/Demo"

pwd
rm -f ./build/LinuxTSANDebug/GraphiteClangCheck
cmake --preset LinuxTSANDebug
cmake --build build/LinuxTSANDebug/ --target format
cmake --build build/LinuxTSANDebug/

export TSAN_OPTIONS=suppressions=../SanitizerSuppresions/TSAN.supp && ./build/LinuxTSANDebug/GraphiteClangCheck

