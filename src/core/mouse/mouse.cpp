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
    double frameScrollX = 0.0;
    double frameScrollY = 0.0;

    GLFWwindow* g_window = nullptr;
    bool mouseLocked = false;
    bool ignoreNextDelta = false;

    void SyncCursorHistory() {
        if (!g_window) return;
        glfwGetCursorPos(g_window, &mouseX, &mouseY);
        lastX = mouseX;
        lastY = mouseY;
        deltaX = 0.0;
        deltaY = 0.0;
    }

    void ScrollCallback(GLFWwindow*, double xoff, double yoff) {
        scrollX += xoff;
        scrollY += yoff;
    }
}

namespace Mouse {

    void Init(GLFWwindow* window) {
        g_window = window;

        glfwSetScrollCallback(window, ScrollCallback);
        SyncCursorHistory();

        buttons[GLFW_MOUSE_BUTTON_LEFT] = ButtonState{};
        buttons[GLFW_MOUSE_BUTTON_RIGHT] = ButtonState{};
        buttons[GLFW_MOUSE_BUTTON_MIDDLE] = ButtonState{};
    }

    void Update() {
        if (!g_window) return;

        glfwGetCursorPos(g_window, &mouseX, &mouseY);

        // Compute relative movement
        deltaX = mouseX - lastX;
        deltaY = mouseY - lastY;

        if (ignoreNextDelta) {
            deltaX = 0.0;
            deltaY = 0.0;
            ignoreNextDelta = false;
        }

        // Update last position
        lastX = mouseX;
        lastY = mouseY;

        // Snapshot per-frame scroll and clear accumulation for next frame.
        frameScrollX = scrollX;
        frameScrollY = scrollY;
        scrollX = 0.0;
        scrollY = 0.0;

        // Update button transitions
        for (auto& [button, state] : buttons) {
            const bool isDown = (glfwGetMouseButton(g_window, button) == GLFW_PRESS);
            state.pressed = (!state.down && isDown);
            state.released = (state.down && !isDown);
            state.down = isDown;
        }
    }

    double X() { return mouseX; }
    double Y() { return mouseY; }

    double DeltaX() { return deltaX; }
    double DeltaY() { return deltaY; }

    bool IsDown(int button) { return buttons[button].down; }
    bool IsPressed(int button) { return buttons[button].pressed; }
    bool IsReleased(int button) { return buttons[button].released; }

    double ScrollX() { return frameScrollX; }
    double ScrollY() { return frameScrollY; }

    void Lock() {
        if (g_window) {
            mouseLocked = true;
            glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            if (glfwRawMouseMotionSupported()) {
                glfwSetInputMode(g_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            }

            SyncCursorHistory();
            ignoreNextDelta = true;
        }
    }

    void Unlock() {
        if (g_window) {
            mouseLocked = false;

            if (glfwRawMouseMotionSupported()) {
                glfwSetInputMode(g_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
            }

            glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            SyncCursorHistory();
            ignoreNextDelta = true;
        }
    }

    bool IsLocked() { return mouseLocked; }
}
