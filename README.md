## Prerequisites

**On MSYS2, CYGWIN, macOS, Linux**:
- gcc
- gdb

**On Windows**:
- Visual Studio

## Build

**On MSYS2, CYGWIN, macOS, Linux**:
- Open Terminal and go to the project directory.
- Run `./build.sh` from the root of the project.

**On Windows**:
- Open Visual Studio Developer Command Prompt and go to the project directory.
- Run `./build.bat` from the root of the project.

## Debug

**On MSYS2, CYGWIN**:
- Open Terminal and go to the project directory.
- Run `gdb build/win32_pong0.exe` from the root of the project.
- On the gdb prompt run `start` to start debugging. If you get a warning / error saying something like `.../crt/crtexewin.c: No such file or directory.`, ignore it.

**On Windows**:
- Open Visual Studio Developer Command Prompt and go to the project directory.
- Run `devenv build/win32_pong0.exe` from the root of the project.

**On macOS**:
- Open Terminal and go to the project directory.
- Run `gdb build/osx_pong0` from the root of the project.

**On Linux**:
- Open Terminal and go to the project directory.
- Run `gdb build/linux_pong0` from the root of the project.

## Run

**On Windows, MSYS2, CYGWIN**
- Open Terminal and go to the project directory.
- Run `./build/win32_pong0.exe`.

**On macOS**:
- Open Terminal and go to the project directory.
- Run `./build/osx_pong0`.

**On Linux**:
- Open Terminal and go to the project directory.
- Run `./build/linux_pong0`.

NOTE: The steps mentioned above are just a couple of ways of setting up the development environment. There are other ways of doing this. For instance, a setup using a combination of VSCode and clang is a good cross-platform option.

