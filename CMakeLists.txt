cmake_minimum_required(VERSION 3.16)
project(MyEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# ---------- Engine ----------
file(GLOB_RECURSE ENGINE_SOURCES src/*.cpp)

add_executable(MyEngine ${ENGINE_SOURCES})

target_link_libraries(MyEngine PRIVATE glfw glad)

# Required on Linux
if (UNIX AND NOT APPLE)
    target_link_libraries(MyEngine PRIVATE dl)
endif()

# ---------------------------------
# Shader auto-copy (NEW)
# ---------------------------------

# Shader source directory (inside src)
set(SHADER_SRC_DIR ${CMAKE_SOURCE_DIR}/src/renderer/shaders)

# Shader destination directory (next to executable)
set(SHADER_DST_DIR ${CMAKE_BINARY_DIR}/renderer/shaders)

# Ensure destination exists
file(MAKE_DIRECTORY ${SHADER_DST_DIR})

# Copy shaders every build (only updates changed files)
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${SHADER_SRC_DIR}
            ${SHADER_DST_DIR}
    COMMENT "Copying shaders to build directory"
)

# Ensure shaders are copied before the executable runs
add_dependencies(MyEngine copy_shaders)
