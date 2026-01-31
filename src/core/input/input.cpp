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

    void RegisterKey(const std::string& name, int key) {
        std::string lower = name;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        nameToKey[lower] = key;
    }

    void RegisterDefaults() {
        RegisterKey("space", GLFW_KEY_SPACE);
        RegisterKey("escape", GLFW_KEY_ESCAPE);
        RegisterKey("enter", GLFW_KEY_ENTER);

        for (char c = 'a'; c <= 'z'; ++c)
            RegisterKey(std::string(1, c), GLFW_KEY_A + (c - 'a'));

        for (int i = 0; i <= 9; ++i)
            RegisterKey(std::to_string(i), GLFW_KEY_0 + i);
    }

    void KeyCallback(GLFWwindow*, int key, int, int action, int) {
        if (key < 0) return;

        auto& state = keys[key];

        if (action == GLFW_PRESS) {
            state.down = true;
            state.pressed = true;
        } else if (action == GLFW_RELEASE) {
            state.down = false;
            state.released = true;
        }
    }
}

namespace Input {

    void Init(GLFWwindow* window) {
        glfwSetKeyCallback(window, KeyCallback);
        RegisterDefaults();
    }

    void Update() {
        for (auto& [_, state] : keys) {
            state.pressed = false;
            state.released = false;
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