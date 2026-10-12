#!/bin/bash
set -euo pipefail

# Build the demo application on Linux using the Demo/CMakePresets.json WindowsNormalRelease preset.
cd "$(dirname "$0")/Demo"

pwd
rm -f ./build/WindowsNormalRelease/GraphiteClangCheck
cmake --preset WindowsNormalRelease && cmake --build build/WindowsNormalRelease/ --target format && cmake --build build/WindowsNormalRelease/

./build/WindowsNormalRelease/GraphiteClangCheck

