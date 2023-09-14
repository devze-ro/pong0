@echo off

mkdir build

pushd build
cl ..\src\win32_pong0.c user32.lib /link /out:win32_pong0.exe
popd

