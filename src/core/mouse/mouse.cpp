#include "mouse.hpp"
#include <GLFW/glfw3.h>
#include <unordered_map>

namespace {

    struct ButtonState {
        bool down = false;
        bool pressed = false;
        bool released = false;
    };

    std::unordered_map<int, ButtonState> buttons;

    double mouseX = 0.0;
    double mouseY = 0.0;
    double lastX = 0.0;
    double lastY = 0.0;
    double deltaX = 0.0;
    double deltaY = 0.0;

    double scrollX = 0.0;
    double scrollY = 0.0;

    void CursorPosCallback(GLFWwindow*, double x, double y) {
        mouseX = x;
        mouseY = y;
    }

    void MouseButtonCallback(GLFWwindow*, int button, int action, int) {
        auto& state = buttons[button];

        if (action == GLFW_PRESS) {
            state.down = true;
            state.pressed = true;
        }
        else if (action == GLFW_RELEASE) {
            state.down = false;
            state.released = true;
        }
    }

    void ScrollCallback(GLFWwindow*, double xoff, double yoff) {
        scrollX += xoff;
        scrollY += yoff;
    }
}

namespace Mouse {

    void Init(GLFWwindow* window) {
        glfwSetCursorPosCallback(window, CursorPosCallback);
        glfwSetMouseButtonCallback(window, MouseButtonCallback);
        glfwSetScrollCallback(window, ScrollCallback);
    }

    void Update() {
        deltaX = mouseX - lastX;
        deltaY = mouseY - lastY;
        lastX = mouseX;
        lastY = mouseY;

        scrollX = 0.0;
        scrollY = 0.0;

        for (auto& [_, state] : buttons) {
            state.pressed = false;
            state.released = false;
        }
    }

    double X() { return mouseX; }
    double Y() { return mouseY; }

    double DeltaX() { return deltaX; }
    double DeltaY() { return deltaY; }

    bool IsDown(int button) {
        return buttons[button].down;
    }

    bool IsPressed(int button) {
        return buttons[button].pressed;
    }

    bool IsReleased(int button) {
        return buttons[button].released;
    }

    double ScrollX() { return scrollX; }
    double ScrollY() { return scrollY; }
}
