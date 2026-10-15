@echo off
setlocal

set COMMON_ARGS=platform=windows use_mingw=yes

echo === Building target=template_debug ===
scons %COMMON_ARGS% target=template_debug
if errorlevel 1 exit /b %errorlevel%

echo === Building target=template_release ===
scons %COMMON_ARGS% target=template_release
if errorlevel 1 exit /b %errorlevel%

echo === All builds complete ===
endlocal