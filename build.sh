#!/bin/bash

mkdir -p build

if [[ "$OSTYPE" == "msys" ]]; then
    gcc src/win32_pong0.c -o ./build/win32_pong0.exe -lgdi32
elif [[ "$OSTYPE" == "cygwin" ]]; then
    gcc src/win32_pong0.c -o ./build/win32_pong0.exe -lgdi32
elif [[ "$OSTYPE" == "darwin"* ]]; then
    gcc -g -O0 -framework Cocoa -framework CoreVideo src/osx_pong0.m -o ./build/osx_pong0
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    gcc src/linux_pong0.c -o ./build/linux_pong0 -lX11
else
    echo "Unsupported Platform: $OSTYPE"
fi

