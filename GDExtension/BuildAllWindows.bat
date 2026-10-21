@echo off
setlocal

scons platform=windows use_mingw=yes target=template_debug
scons platform=windows use_mingw=yes target=template_release

echo === All builds complete ===
endlocal