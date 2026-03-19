#!/bin/bash
set -euo pipefail

DIST_DIR="dist"
EXE_NAME="fungi.exe"

if ! command -v x86_64-w64-mingw32-g++ >/dev/null 2>&1; then
    echo "ERROR: x86_64-w64-mingw32-g++ not found"
    echo "Install with: sudo apt install -y mingw-w64"
    exit 1
fi

CC=x86_64-w64-mingw32-gcc \
CXX=x86_64-w64-mingw32-g++ \
bash build.sh windows

mkdir -p "$DIST_DIR"

# Bundle common MinGW runtime DLLs so the .exe can run on Windows without extra setup.
RUNTIME_DLLS=(
    libstdc++-6.dll
    libgcc_s_seh-1.dll
    libwinpthread-1.dll
    libssp-0.dll
)

for dll in "${RUNTIME_DLLS[@]}"; do
    dll_path="$(x86_64-w64-mingw32-g++ -print-file-name="$dll")"
    if [ -n "$dll_path" ] && [ "$dll_path" != "$dll" ] && [ -f "$dll_path" ]; then
        cp "$dll_path" "$DIST_DIR/"
    fi
done

if [ ! -f "$DIST_DIR/$EXE_NAME" ]; then
    echo "ERROR: expected output not found: $DIST_DIR/$EXE_NAME"
    exit 1
fi

echo "Windows package ready in $DIST_DIR/"