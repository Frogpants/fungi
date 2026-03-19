# Fungi

Simple OpenGL 3.3 project with GLFW + GLAD.

## Commands (Community Style)

Build native executable for current OS:
```bash
make
```

Run executable:
```bash
make run
```

Clean artifacts:
```bash
make clean
```

Show all targets:
```bash
make help
```

## Build Targets

- `make` or `make native`: native build for your current OS
- `make linux`: force Linux native build
- `make macos`: force macOS native build
- `make windows`: build Windows `.exe` (native on Windows, cross-build on Linux)
- `make windows-cross`: cross-build Windows `.exe` from Linux (MinGW cross compiler)

## Outputs

- Linux/macOS: `dist/fungi`
- Windows: `dist/fungi.exe`

## Dependencies

No manual third-party graphics library install is required.
`make` automatically bootstraps GLFW + GLAD + STB source files into the repo on first build.

Linux cross-compiling Windows:
```bash
sudo apt install -y mingw-w64
```

Linux native toolchain prerequisites (compiler + system OpenGL/X11 dev libs):
```bash
sudo apt update
sudo apt install -y build-essential clang git curl tar \
	libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev libxxf86vm-dev libgl1-mesa-dev
```

macOS (Homebrew):
```bash
brew install llvm git curl
```

Windows (MSYS2 MinGW64 shell):
```bash
pacman -S --needed mingw-w64-x86_64-toolchain git curl
```

Cross-build now auto-copies common MinGW runtime DLLs next to `dist/fungi.exe`.

## No CMake

This project no longer uses CMake. The workflow is make + shell scripts only.

## Controls

- `W/A/S/D`: move
- `Q/E`: move down/up
- Left mouse click: lock cursor
- Mouse move: look around
- `Esc`: unlock cursor
- `1`: FPS quality = Performance (240 FPS target)
- `2`: FPS quality = Balanced (120 FPS target)
- `3`: FPS quality = Quality (60 FPS target)

Window title shows live FPS and current quality preset.