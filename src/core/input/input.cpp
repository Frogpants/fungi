#include "input.hpp"
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <algorithm>

namespace {

    struct KeyState {
        bool down = false;
        bool pressed = false;
        bool released = false;
    };

    std::unordered_map<int, KeyState> keys;
    std::unordered_map<std::string, int> nameToKey;
    GLFWwindow* g_window = nullptr;

    void RegisterKey(const std::string& name, int key) {
        std::string lower = name;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        nameToKey[lower] = key;
    }

    void RegisterDefaults() {
        RegisterKey("space", GLFW_KEY_SPACE);
        RegisterKey("escape", GLFW_KEY_ESCAPE);
        RegisterKey("enter", GLFW_KEY_ENTER);
        RegisterKey("tab", GLFW_KEY_TAB);
        RegisterKey("shift", GLFW_KEY_LEFT_SHIFT);
        RegisterKey("ctrl", GLFW_KEY_LEFT_CONTROL);

        for (char c = 'a'; c <= 'z'; ++c)
            RegisterKey(std::string(1, c), GLFW_KEY_A + (c - 'a'));

        for (int i = 0; i <= 9; ++i)
            RegisterKey(std::to_string(i), GLFW_KEY_0 + i);

        RegisterKey("up", GLFW_KEY_UP);
        RegisterKey("down", GLFW_KEY_DOWN);
        RegisterKey("left", GLFW_KEY_LEFT);
        RegisterKey("right", GLFW_KEY_RIGHT);
    }
}

namespace Input {

    void Init(GLFWwindow* window) {
        g_window = window;
        RegisterDefaults();
    }

    void Update() {
        if (!g_window) return;

        for (const auto& [_, key] : nameToKey) {
            const int action = glfwGetKey(g_window, key);
            const bool isDown = (action == GLFW_PRESS || action == GLFW_REPEAT);

            auto& state = keys[key];
            state.pressed = (!state.down && isDown);
            state.released = (state.down && !isDown);
            state.down = isDown;
        }
    }

    bool IsDown(int key) {
        return keys[key].down;
    }

    bool IsPressed(int key) {
        return keys[key].pressed;
    }

    bool IsReleased(int key) {
        return keys[key].released;
    }

    int Resolve(const std::string& name) {
        std::string lower = name;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (nameToKey.count(lower))
            return nameToKey[lower];
        return -1;
    }

    bool IsDown(const std::string& name) {
        int key = Resolve(name);
        return key != -1 && IsDown(key);
    }

    bool IsPressed(const std::string& name) {
        int key = Resolve(name);
        return key != -1 && IsPressed(key);
    }

    bool IsReleased(const std::string& name) {
        int key = Resolve(name);
        return key != -1 && IsReleased(key);
    }
}