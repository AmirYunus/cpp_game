@echo off

mkdir ..\build
pushd ..\build
cl -FC -Zi ..\code\win32.cpp user32.lib gdi32.lib
popd
