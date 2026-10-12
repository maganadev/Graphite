#!/bin/bash
export PATH="/ucrt64/bin:/usr/bin:/bin:$PATH"
cd "$(cygpath 'C:\Users\WindowsUser\Repos\GraphiteClangCheck\Demo\')"

pwd
rm -f ./build/WindowsNormalDebug/GraphiteClangCheck.exe
cmake --preset WindowsNormalDebug && cmake --build build/WindowsNormalDebug/ --target format && cmake --build build/WindowsNormalDebug/
./build/WindowsNormalDebug/GraphiteClangCheck.exe
