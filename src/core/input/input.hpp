#pragma once
#include <string>

struct GLFWwindow;

namespace Input {

    // Engine-level lifecycle
    void Init(GLFWwindow* window);
    void Update();

    // Query by keycode
    bool IsDown(int key);
    bool IsPressed(int key);
    bool IsReleased(int key);

    // Query by name
    bool IsDown(const std::string& key);
    bool IsPressed(const std::string& key);
    bool IsReleased(const std::string& key);
}
