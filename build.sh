#!/bin/bash

mkdir -p build

if [[ "$OSTYPE" == "darwin"* ]]; then
    gcc -framework Cocoa src/osx_pong0.m -o ./build/osx_pong0
elif [[ "$OSTYPE" == "cygwin" ]]; then
    gcc src/win32_pong0.c -o ./build/win32_pong0.exe
elif [[ "$OSTYPE" == "msys" ]]; then
    gcc src/win32_pong0.c -o ./build/win32_pong0.exe
else
    echo "Unsupported Platform: $OSTYPE"
fi

