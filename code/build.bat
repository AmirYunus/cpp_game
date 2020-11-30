@echo off

mkdir ..\build
pushd ..\build
cl -Zi ..\code\win32.cpp user32.lib
popd
