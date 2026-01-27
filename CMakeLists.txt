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
