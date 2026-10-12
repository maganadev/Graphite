#!/bin/bash
set -euo pipefail

export PATH="/ucrt64/bin:/usr/bin:/bin:$PATH"
cd "$(dirname "$0")/Demo"

pwd
rm -f ./build/WindowsNormalRelease/GraphiteClangCheck.exe
cmake --preset WindowsNormalRelease && cmake --build build/WindowsNormalRelease/ --target format && cmake --build build/WindowsNormalRelease/

./build/WindowsNormalRelease/GraphiteClangCheck.exe