cmake_minimum_required(VERSION 3.16)
project(MyEngine LANGUAGES CXX)

# ----------------------------
# C++ Standard
# ----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# ----------------------------
# Dependencies
# ----------------------------

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
)
FetchContent_MakeAvailable(glfw)

# GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# STB
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stb)

# ----------------------------
# Engine Sources
# ----------------------------
file(GLOB_RECURSE ENGINE_CPP
    CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE ENGINE_HPP
    CONFIGURE_DEPENDS
    src/*.hpp
    src/*.h
)

# ----------------------------
# Executable (NAME = run)
# ----------------------------
add_executable(run
    ${ENGINE_CPP}
    ${ENGINE_HPP}
)

# ----------------------------
# Include Directories
# ----------------------------
target_include_directories(run PRIVATE
    src
    ${glfw_SOURCE_DIR}/include
    ${glad_SOURCE_DIR}/include
    ${stb_SOURCE_DIR}
)

# ----------------------------
# Link Libraries
# ----------------------------
target_link_libraries(run PRIVATE
    glfw
    glad
)

# ----------------------------
# Platform-specific OpenGL
# ----------------------------
if(UNIX AND NOT APPLE)
    target_link_libraries(run PRIVATE
        dl
        pthread
        X11
        Xrandr
        Xi
        Xxf86vm
        Xcursor
        GL
    )
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    find_library(OPENGL_LIBRARY OpenGL)

    target_link_libraries(run PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
        ${OPENGL_LIBRARY}
    )
elseif(WIN32)
    target_link_libraries(run PRIVATE opengl32)
endif()

# ----------------------------
# Asset Copying
# ----------------------------
set(SHADER_SRC_DIR ${CMAKE_SOURCE_DIR}/src/renderer/shaders)
set(SHADER_DST_DIR ${CMAKE_BINARY_DIR}/renderer/shaders)

add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_DST_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${SHADER_SRC_DIR}
            ${SHADER_DST_DIR}
    COMMENT "Copying shaders"
)

set(MODEL_SRC_DIR ${CMAKE_SOURCE_DIR}/src/game/models)
set(MODEL_DST_DIR ${CMAKE_BINARY_DIR}/game/models)

add_custom_target(copy_models ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MODEL_DST_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${MODEL_SRC_DIR}
            ${MODEL_DST_DIR}
    COMMENT "Copying models"
)

add_dependencies(run copy_shaders copy_models)