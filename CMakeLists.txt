cmake_minimum_required(VERSION 3.16)
project(MyEngine LANGUAGES CXX)

# ----------------------------
# C++ Standard
# ----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----------------------------
# Warnings (catch UB early)
# ----------------------------
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include(FetchContent)

# ----------------------------
# GLFW
# ----------------------------
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
)
FetchContent_MakeAvailable(glfw)

# ----------------------------
# GLAD (CORRECT)
# ----------------------------
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
)

# Tell GLAD what to generate
set(GLAD_PROFILE "core")
set(GLAD_API "gl=3.3")
set(GLAD_GENERATOR "c")
set(GLAD_EXTENSIONS "")

FetchContent_MakeAvailable(glad)

# ----------------------------
# STB (HEADER-ONLY)
# ----------------------------
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stb)

# ----------------------------
# Engine Sources
# ----------------------------
file(GLOB_RECURSE ENGINE_CPP CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE ENGINE_HPP CONFIGURE_DEPENDS src/*.hpp src/*.h)

# ----------------------------
# Executable
# ----------------------------
add_executable(run
    ${ENGINE_CPP}
    ${ENGINE_HPP}
)

# ----------------------------
# Includes
# ----------------------------
target_include_directories(run PRIVATE
    src
    ${stb_SOURCE_DIR}
)

# ----------------------------
# OpenGL (PORTABLE)
# ----------------------------
find_package(OpenGL REQUIRED)

# ----------------------------
# Link Libraries
# ----------------------------
target_link_libraries(run PRIVATE
    glfw
    glad
    OpenGL::GL
)

# ----------------------------
# Platform Extras
# ----------------------------
if (UNIX AND NOT APPLE)
    target_link_libraries(run PRIVATE
        dl
        pthread
        X11
        Xrandr
        Xi
        Xcursor
        Xxf86vm
    )
elseif (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)

    target_link_libraries(run PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
    )
elseif (WIN32)
    target_link_libraries(run PRIVATE opengl32)
endif()

# ----------------------------
# Assets (RUNTIME-CRITICAL)
# ----------------------------
set(ASSET_OUTPUT_DIR $<TARGET_FILE_DIR:run>)

add_custom_command(TARGET run POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/renderer/shaders
        ${ASSET_OUTPUT_DIR}/renderer/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/game/models
        ${ASSET_OUTPUT_DIR}/game/models
    COMMENT "Copying runtime assets"
)