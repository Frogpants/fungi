#!/bin/bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

download_file() {
    local url="$1"
    local out="$2"
    if command -v curl >/dev/null 2>&1; then
        curl -fsSL "$url" -o "$out"
    elif command -v wget >/dev/null 2>&1; then
        wget -qO "$out" "$url"
    else
        echo "ERROR: curl or wget is required to download dependencies."
        exit 1
    fi
}

clone_repo() {
    local repo_url="$1"
    local out_dir="$2"
    local ref="${3:-}"

    if ! command -v git >/dev/null 2>&1; then
        echo "ERROR: git is required to clone dependencies."
        exit 1
    fi

    if [ -n "$ref" ]; then
        git clone --depth 1 --branch "$ref" "$repo_url" "$out_dir"
    else
        git clone --depth 1 "$repo_url" "$out_dir"
    fi
}

init_repo() {
    local repo_dir="$1"

    # Keep dependency checkouts self-contained by initializing nested modules.
    if [ -d "$repo_dir/.git" ]; then
        git -C "$repo_dir" submodule sync --recursive || true
        git -C "$repo_dir" submodule update --init --recursive || true
    fi
}

bootstrap_glfw() {
    if [ -f "$ROOT_DIR/third_party/glfw/include/GLFW/glfw3.h" ]; then
        return
    fi

    mkdir -p "$ROOT_DIR/third_party"

    if command -v git >/dev/null 2>&1; then
        git clone --depth 1 --branch 3.3.9 https://github.com/glfw/glfw.git "$ROOT_DIR/third_party/glfw"
        init_repo "$ROOT_DIR/third_party/glfw"
        return
    fi

    local archive="$ROOT_DIR/third_party/glfw-3.3.9.tar.gz"
    download_file "https://github.com/glfw/glfw/archive/refs/tags/3.3.9.tar.gz" "$archive"
    tar -xzf "$archive" -C "$ROOT_DIR/third_party"
    rm -f "$archive"
    mv "$ROOT_DIR/third_party/glfw-3.3.9" "$ROOT_DIR/third_party/glfw"
}

bootstrap_glad_stb() {
    mkdir -p "$ROOT_DIR/third_party"
    mkdir -p "$ROOT_DIR/src/external/include/glad"
    mkdir -p "$ROOT_DIR/src/external/include/KHR"
    mkdir -p "$ROOT_DIR/src/external/src"

    if [ ! -d "$ROOT_DIR/third_party/glad" ]; then
        clone_repo "https://github.com/Dav1dde/glad.git" "$ROOT_DIR/third_party/glad" "v0.1.34"
    fi
    init_repo "$ROOT_DIR/third_party/glad"

    # The glad repository is primarily a generator. Create pre-generated C files if needed.
    if [ ! -f "$ROOT_DIR/third_party/glad/include/glad/glad.h" ] ||
       [ ! -f "$ROOT_DIR/third_party/glad/include/KHR/khrplatform.h" ] ||
       [ ! -f "$ROOT_DIR/third_party/glad/src/glad.c" ]; then
        if command -v python3 >/dev/null 2>&1; then
            local glad_out="$ROOT_DIR/third_party/glad-generated"
            rm -rf "$glad_out"
            mkdir -p "$glad_out"

            if (
                cd "$ROOT_DIR/third_party/glad"
                python3 -m glad --profile core --api gl=3.3 --generator c --spec gl --out-path "$glad_out"
            ); then
                mkdir -p "$ROOT_DIR/third_party/glad/include/glad"
                mkdir -p "$ROOT_DIR/third_party/glad/include/KHR"
                mkdir -p "$ROOT_DIR/third_party/glad/src"

                cp "$glad_out/include/glad/glad.h" "$ROOT_DIR/third_party/glad/include/glad/glad.h"
                cp "$glad_out/include/KHR/khrplatform.h" "$ROOT_DIR/third_party/glad/include/KHR/khrplatform.h"
                cp "$glad_out/src/glad.c" "$ROOT_DIR/third_party/glad/src/glad.c"
            fi
        fi

        if [ ! -f "$ROOT_DIR/third_party/glad/include/glad/glad.h" ] ||
           [ ! -f "$ROOT_DIR/third_party/glad/include/KHR/khrplatform.h" ] ||
           [ ! -f "$ROOT_DIR/third_party/glad/src/glad.c" ]; then
            local prebuilt_glad="$ROOT_DIR/third_party/glad-prebuilt"
            local prebuilt_include_root=""

            if [ ! -d "$prebuilt_glad" ]; then
                clone_repo "https://github.com/JoeyDeVries/LearnOpenGL.git" "$prebuilt_glad"
            fi
            init_repo "$prebuilt_glad"

            if [ -f "$prebuilt_glad/includes/glad/glad.h" ] &&
               [ -f "$prebuilt_glad/includes/KHR/khrplatform.h" ] &&
               [ -f "$prebuilt_glad/src/glad.c" ]; then
                prebuilt_include_root="$prebuilt_glad/includes"
            elif [ -f "$prebuilt_glad/include/glad/glad.h" ] &&
                 [ -f "$prebuilt_glad/include/KHR/khrplatform.h" ] &&
                 [ -f "$prebuilt_glad/src/glad.c" ]; then
                prebuilt_include_root="$prebuilt_glad/include"
            else
                echo "ERROR: could not locate pre-generated GLAD files in $prebuilt_glad."
                exit 1
            fi

            mkdir -p "$ROOT_DIR/third_party/glad/include/glad"
            mkdir -p "$ROOT_DIR/third_party/glad/include/KHR"
            mkdir -p "$ROOT_DIR/third_party/glad/src"

            cp "$prebuilt_include_root/glad/glad.h" "$ROOT_DIR/third_party/glad/include/glad/glad.h"
            cp "$prebuilt_include_root/KHR/khrplatform.h" "$ROOT_DIR/third_party/glad/include/KHR/khrplatform.h"
            cp "$prebuilt_glad/src/glad.c" "$ROOT_DIR/third_party/glad/src/glad.c"
        fi
    fi

    if [ ! -f "$ROOT_DIR/src/external/include/glad/glad.h" ] ||
       [ ! -f "$ROOT_DIR/src/external/include/KHR/khrplatform.h" ] ||
       [ ! -f "$ROOT_DIR/src/external/src/glad.c" ]; then
        cp "$ROOT_DIR/third_party/glad/include/glad/glad.h" "$ROOT_DIR/src/external/include/glad/glad.h"
        cp "$ROOT_DIR/third_party/glad/include/KHR/khrplatform.h" "$ROOT_DIR/src/external/include/KHR/khrplatform.h"
        cp "$ROOT_DIR/third_party/glad/src/glad.c" "$ROOT_DIR/src/external/src/glad.c"
    fi

    if [ ! -d "$ROOT_DIR/third_party/stb" ]; then
        clone_repo "https://github.com/nothings/stb.git" "$ROOT_DIR/third_party/stb"
    fi
    init_repo "$ROOT_DIR/third_party/stb"

    if [ ! -f "$ROOT_DIR/src/external/include/stb_image.h" ]; then
        cp "$ROOT_DIR/third_party/stb/stb_image.h" "$ROOT_DIR/src/external/include/stb_image.h"
    fi
}

bootstrap_glfw
bootstrap_glad_stb

echo "Dependencies ready."
