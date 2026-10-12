#!/bin/bash
set -euo pipefail

# Build the demo application on Linux using the Demo/CMakePresets.json LinuxNormalRelease preset.
cd "$(dirname "$0")/Demo"

pwd
rm -f ./build/LinuxNormalRelease/GraphiteClangCheck
cmake --preset LinuxNormalRelease
cmake --build build/LinuxNormalRelease/ --target format
cmake --build build/LinuxNormalRelease/

./build/LinuxNormalRelease/GraphiteClangCheck

