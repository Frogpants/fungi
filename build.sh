#!/bin/bash
set -euo pipefail

DIST_DIR="dist"
OBJ_DIR="$DIST_DIR/obj"
BIN_BASENAME="fungi"
TARGET_OS="${1:-windows}"

if [ "$TARGET_OS" != "windows" ]; then
    echo "ERROR: this simplified project only supports: bash build.sh windows"
    exit 1
fi

bash scripts/bootstrap_deps.sh

mkdir -p "$DIST_DIR" "$OBJ_DIR"

CC_BIN="${CC:-gcc}"
CXX_BIN="${CXX:-g++}"
INCLUDE_FLAGS="-Ithird_party/glfw/include -Ithird_party/glfw/src -Isrc -Isrc/external/include"
CFLAGS="-O2 -Wall -Wextra -D_GLFW_WIN32 $INCLUDE_FLAGS"
CXXFLAGS="-std=c++17 -O2 -Wall -Wextra -Wpedantic $INCLUDE_FLAGS"

C_SOURCES=(
    third_party/glfw/src/context.c
    third_party/glfw/src/init.c
    third_party/glfw/src/input.c
    third_party/glfw/src/monitor.c
    third_party/glfw/src/vulkan.c
    third_party/glfw/src/window.c
    third_party/glfw/src/egl_context.c
    third_party/glfw/src/osmesa_context.c
    third_party/glfw/src/wgl_context.c
    third_party/glfw/src/win32_init.c
    third_party/glfw/src/win32_joystick.c
    third_party/glfw/src/win32_monitor.c
    third_party/glfw/src/win32_thread.c
    third_party/glfw/src/win32_time.c
    third_party/glfw/src/win32_window.c
    src/external/src/glad.c
)

CPP_SOURCES=(
    src/main.cpp
)

OBJECTS=()

for src in "${C_SOURCES[@]}"; do
    obj="$OBJ_DIR/$(echo "$src" | tr '/.' '__').o"
    "$CC_BIN" -c "$src" -o "$obj" $CFLAGS
    OBJECTS+=("$obj")
done

for src in "${CPP_SOURCES[@]}"; do
    obj="$OBJ_DIR/$(echo "$src" | tr '/.' '__').o"
    "$CXX_BIN" -c "$src" -o "$obj" $CXXFLAGS
    OBJECTS+=("$obj")
done

"$CXX_BIN" "${OBJECTS[@]}" -o "$DIST_DIR/${BIN_BASENAME}.exe" \
    -mconsole -lopengl32 -lgdi32 -luser32 -lkernel32 -lshell32

echo "Build complete: $DIST_DIR/${BIN_BASENAME}.exe"
