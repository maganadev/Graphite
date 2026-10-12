#!/bin/bash
set -euo pipefail

# Build the demo application on Linux using the Demo/CMakePresets.json LinuxASANDebug preset.
cd "$(dirname "$0")/Demo"

pwd
rm -f ./build/LinuxASANDebug/GraphiteClangCheck
cmake --preset LinuxASANDebug
cmake --build build/LinuxASANDebug/ --target format
cmake --build build/LinuxASANDebug/

export ASAN_OPTIONS=suppressions=../SanitizerSuppresions/ASAN.supp && export LSAN_OPTIONS=suppressions=../SanitizerSuppresions/LSAN.supp && ./build/LinuxASANDebug/GraphiteClangCheck
