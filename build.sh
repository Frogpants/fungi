#!/bin/bash
set -euo pipefail

DIST_DIR="dist"
TARGET_OS="${1:-auto}"
OBJ_DIR="$DIST_DIR/obj"
BIN_BASENAME="fungi"

CPP_SOURCES=(
    src/main.cpp
    src/core/input/input.cpp
    src/core/mouse/mouse.cpp
    src/renderer/render.cpp
    src/physics/physics.cpp
    src/external/stb_image.cpp
)

COMMON_GLFW_SOURCES=(
    third_party/glfw/src/context.c
    third_party/glfw/src/init.c
    third_party/glfw/src/input.c
    third_party/glfw/src/monitor.c
    third_party/glfw/src/vulkan.c
    third_party/glfw/src/window.c
    third_party/glfw/src/egl_context.c
    third_party/glfw/src/osmesa_context.c
)

bash scripts/bootstrap_deps.sh

mkdir -p "$DIST_DIR" "$DIST_DIR/renderer" "$DIST_DIR/game" "$OBJ_DIR"

copy_assets() {
    cp -r src/renderer/shaders "$DIST_DIR/renderer/"
    cp -r src/game/models "$DIST_DIR/game/"
}

compile_objects() {
    local cc="$1"
    local cxx="$2"
    local cflags="$3"
    local cxxflags="$4"
    shift 4
    local c_sources=("$@")

    local objects=()

    for src in "${c_sources[@]}"; do
        local obj="$OBJ_DIR/$(echo "$src" | tr '/.' '__').o"
        "$cc" -c "$src" -o "$obj" $cflags
        objects+=("$obj")
    done

    for src in "${CPP_SOURCES[@]}"; do
        local obj="$OBJ_DIR/$(echo "$src" | tr '/.' '__').o"
        "$cxx" -c "$src" -o "$obj" $cxxflags
        objects+=("$obj")
    done

    printf '%s\n' "${objects[@]}"
}

build_linux() {
    echo "======================================"
    echo " Building fungi Linux executable "
    echo "======================================"

    local cc="${CC:-gcc}"
    local cxx="${CXX:-g++}"
    local include_flags="-Ithird_party/glfw/include -Ithird_party/glfw/src -Isrc -Isrc/external/include"
    local cflags="-O2 -Wall -Wextra -D_GLFW_X11 $include_flags"
    local cxxflags="-std=c++17 -O2 -Wall -Wextra -Wpedantic $include_flags"
    local platform_sources=(
        third_party/glfw/src/glx_context.c
        third_party/glfw/src/linux_joystick.c
        third_party/glfw/src/posix_thread.c
        third_party/glfw/src/posix_time.c
        third_party/glfw/src/x11_init.c
        third_party/glfw/src/x11_monitor.c
        third_party/glfw/src/x11_window.c
        third_party/glfw/src/xkb_unicode.c
        src/external/src/glad.c
    )

    mapfile -t objects < <(compile_objects "$cc" "$cxx" "$cflags" "$cxxflags" "${COMMON_GLFW_SOURCES[@]}" "${platform_sources[@]}")

    "$cxx" "${objects[@]}" -o "$DIST_DIR/$BIN_BASENAME" \
        -ldl -pthread -lGL -lm -lX11 -lXrandr -lXi -lXcursor -lXxf86vm -lXinerama

    copy_assets
    echo "Build complete: $DIST_DIR/$BIN_BASENAME"
}

build_macos() {
    echo "======================================"
    echo " Building fungi macOS executable "
    echo "======================================"

    local cc="${CC:-clang}"
    local cxx="${CXX:-clang++}"
    local include_flags="-Ithird_party/glfw/include -Ithird_party/glfw/src -Isrc -Isrc/external/include"
    local cflags="-O2 -Wall -Wextra -D_GLFW_COCOA $include_flags"
    local cxxflags="-std=c++17 -O2 -Wall -Wextra -Wpedantic $include_flags"
    local platform_sources=(
        third_party/glfw/src/cocoa_init.m
        third_party/glfw/src/cocoa_joystick.m
        third_party/glfw/src/cocoa_monitor.m
        third_party/glfw/src/cocoa_time.c
        third_party/glfw/src/cocoa_window.m
        third_party/glfw/src/nsgl_context.m
        third_party/glfw/src/posix_thread.c
        src/external/src/glad.c
    )

    mapfile -t objects < <(compile_objects "$cc" "$cxx" "$cflags" "$cxxflags" "${COMMON_GLFW_SOURCES[@]}" "${platform_sources[@]}")

    "$cxx" "${objects[@]}" -o "$DIST_DIR/$BIN_BASENAME" \
        -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

    copy_assets
    echo "Build complete: $DIST_DIR/$BIN_BASENAME"
}

build_windows_native() {
    echo "======================================"
    echo " Building fungi Windows executable "
    echo "======================================"

    local cc="${CC:-gcc}"
    local cxx="${CXX:-g++}"
    local include_flags="-Ithird_party/glfw/include -Ithird_party/glfw/src -Isrc -Isrc/external/include"
    local cflags="-O2 -Wall -Wextra -D_GLFW_WIN32 $include_flags"
    local cxxflags="-std=c++17 -O2 -Wall -Wextra -Wpedantic $include_flags"
    local platform_sources=(
        third_party/glfw/src/wgl_context.c
        third_party/glfw/src/win32_init.c
        third_party/glfw/src/win32_joystick.c
        third_party/glfw/src/win32_monitor.c
        third_party/glfw/src/win32_thread.c
        third_party/glfw/src/win32_time.c
        third_party/glfw/src/win32_window.c
        src/external/src/glad.c
    )

    mapfile -t objects < <(compile_objects "$cc" "$cxx" "$cflags" "$cxxflags" "${COMMON_GLFW_SOURCES[@]}" "${platform_sources[@]}")

    "$cxx" "${objects[@]}" -o "$DIST_DIR/${BIN_BASENAME}.exe" \
        -lopengl32 -lgdi32 -luser32 -lkernel32 -lshell32

    copy_assets
    echo "Build complete: $DIST_DIR/${BIN_BASENAME}.exe"
}

if [ "$TARGET_OS" = "auto" ]; then
    case "$(uname -s)" in
        Linux*) TARGET_OS="linux" ;;
        Darwin*) TARGET_OS="macos" ;;
        MINGW*|MSYS*|CYGWIN*) TARGET_OS="windows" ;;
        *)
            echo "ERROR: unsupported host OS: $(uname -s)"
            echo "Use: bash build.sh linux|macos|windows"
            exit 1
            ;;
    esac
fi

case "$TARGET_OS" in
    linux) build_linux ;;
    macos) build_macos ;;
    windows) build_windows_native ;;
    *)
        echo "ERROR: unknown target OS: $TARGET_OS"
        echo "Use: bash build.sh linux|macos|windows"
        exit 1
        ;;
esac
