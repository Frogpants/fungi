#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

struct WindowContext {
    GLFWwindow* window = nullptr;
};

inline bool InitWindowContext(WindowContext& ctx, int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    ctx.window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!ctx.window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(ctx.window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(ctx.window);
        ctx.window = nullptr;
        glfwTerminate();
        return false;
    }

    return true;
}

inline void ShutdownWindowContext(WindowContext& ctx) {
    if (ctx.window) {
        glfwDestroyWindow(ctx.window);
        ctx.window = nullptr;
    }
    glfwTerminate();
}
