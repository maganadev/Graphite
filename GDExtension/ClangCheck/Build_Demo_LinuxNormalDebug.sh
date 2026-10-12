#!/bin/bash
set -euo pipefail

# Build the demo application on Linux using the Demo/CMakePresets.json LinuxNormalDebug preset.
cd "$(dirname "$0")/Demo"

pwd
rm -f ./build/LinuxNormalDebug/GraphiteClangCheck
cmake --preset LinuxNormalDebug
cmake --build build/LinuxNormalDebug/ --target format
cmake --build build/LinuxNormalDebug/

./build/LinuxNormalDebug/GraphiteClangCheck

