#pragma once

#include <GLFW/glfw3.h>

#include "../core/input/input.hpp"
#include "../core/mouse/mouse.hpp"

inline float gamemode = 0.0f;

namespace Manager {

    inline GLFWwindow* window = nullptr;

    inline void Init(GLFWwindow* win) {
        window = win;

        Input::Init(window);
        Mouse::Init(window);
    }

    inline void Update() {
        Input::Update();
        Mouse::Update();
    }
}
