cmake_minimum_required(VERSION 3.16)
project(MyEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# ---------- GLFW ----------
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
)
FetchContent_MakeAvailable(glfw)

# ---------- GLAD ----------
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# ---------- STB_IMAGE ----------
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stb)

# ---------- Engine Sources ----------
file(GLOB_RECURSE ENGINE_SOURCES src/*.cpp src/*.h)

# ---------- Create executable ----------
add_executable(MyEngine ${ENGINE_SOURCES})

# ---------- Include Directories ----------
target_include_directories(MyEngine PRIVATE
    ${glfw_SOURCE_DIR}/include
    ${glad_SOURCE_DIR}/include
    ${stb_SOURCE_DIR}    # for stb_image.h
    src
)

# ---------- Link Libraries ----------
target_link_libraries(MyEngine PRIVATE glfw glad)

# ---------- OS-specific dependencies ----------
if(UNIX AND NOT APPLE)
    target_link_libraries(MyEngine PRIVATE dl pthread X11 Xrandr Xi Xxf86vm Xcursor)
endif()

if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    find_library(OPENGL_LIBRARY OpenGL)

    target_link_libraries(MyEngine PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
        ${OPENGL_LIBRARY}
    )
endif()

if(WIN32)
    target_link_libraries(MyEngine PRIVATE opengl32)
endif()

# ---------------------------------
# Shader auto-copy
# ---------------------------------
set(SHADER_SRC_DIR ${CMAKE_SOURCE_DIR}/src/renderer/shaders)
set(SHADER_DST_DIR ${CMAKE_BINARY_DIR}/renderer/shaders)
file(MAKE_DIRECTORY ${SHADER_DST_DIR})

add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${SHADER_SRC_DIR}
            ${SHADER_DST_DIR}
    COMMENT "Copying shaders to build directory"
)

# ---------------------------------
# Model auto-copy
# ---------------------------------
set(MODEL_SRC_DIR ${CMAKE_SOURCE_DIR}/src/game/models)
set(MODEL_DST_DIR ${CMAKE_BINARY_DIR}/game/models)
file(MAKE_DIRECTORY ${MODEL_DST_DIR})

add_custom_target(copy_models ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${MODEL_SRC_DIR}
            ${MODEL_DST_DIR}
    COMMENT "Copying models to build directory"
)

# Ensure assets are copied before building
add_dependencies(MyEngine copy_shaders copy_models)

# Optional: post-build copy to binary folder (if needed)
add_custom_command(TARGET MyEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${SHADER_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/renderer/shaders
)
add_custom_command(TARGET MyEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${MODEL_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/game/models
)
