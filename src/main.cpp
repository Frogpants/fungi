#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "./renderer/render.hpp"
#include "./renderer/projection.hpp"

/* ===============================
   File Utilities
================================ */

std::string LoadFile(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "FAILED TO OPEN FILE: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/* ===============================
   Shader Utilities
================================ */

GLuint CompileShader(const char* path, GLenum type) {
    std::string src = LoadFile(path);
    if (src.empty()) return 0;

    const char* csrc = src.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &csrc, nullptr);
    glCompileShader(shader);

    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, log);
        std::cerr << "SHADER COMPILE ERROR (" << path << "):\n" << log << std::endl;
    }

    return shader;
}

GLuint CreateShaderProgram(const char* vert, const char* frag) {
    GLuint vs = CompileShader(vert, GL_VERTEX_SHADER);
    GLuint fs = CompileShader(frag, GL_FRAGMENT_SHADER);

    if (!vs || !fs) return 0;

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint ok;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(program, 1024, nullptr, log);
        std::cerr << "SHADER LINK ERROR:\n" << log << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

/* ===============================
   Globals
================================ */

GLuint postShader = 0;
GLuint sceneFBO = 0;
GLuint sceneTexture = 0;
GLuint fullscreenVAO = 0;

std::vector<Triangle> scene;

/* ===============================
   Scene
================================ */

void InitScene() {
    scene.push_back({
        { -1, -1, 3 },
        {  1, -1, 3 },
        {  0,  1, 3 }
    });
}

/* ===============================
   Framebuffer
================================ */

void InitFramebuffer(int w, int h) {
    glGenFramebuffers(1, &sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

    glGenTextures(1, &sceneTexture);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0,
        GL_RGB,
        w, h,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        sceneTexture,
        0
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "FRAMEBUFFER INCOMPLETE\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Fullscreen triangle VAO (no VBO needed)
    glGenVertexArrays(1, &fullscreenVAO);
    glBindVertexArray(fullscreenVAO);
}

/* ===============================
   Rendering
================================ */

void Render(GLFWwindow* window) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    UpdVals();

    /* ---- Render scene to texture ---- */
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
    glViewport(0, 0, w, h);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    RenderFrame(scene, w, h);

    /* ---- Post process to screen ---- */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(postShader);
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glUniform1i(glGetUniformLocation(postShader, "screenTexture"), 0);
    glUniform2f(glGetUniformLocation(postShader, "resolution"), (float)w, (float)h);
    glUniform1f(glGetUniformLocation(postShader, "time"), (float)glfwGetTime());

    glBindVertexArray(fullscreenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

/* ===============================
   GLFW
================================ */

void framebuffer_size_callback(GLFWwindow*, int w, int h) {
    glViewport(0, 0, w, h);
}

/* ===============================
   Main
================================ */

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =
        glfwCreateWindow(1280, 720, "Mama's Bakeria", nullptr, nullptr);

    if (!window) {
        std::cerr << "Window creation failed\n";
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD init failed\n";
        return -1;
    }

    glfwSwapInterval(1);

    postShader = CreateShaderProgram(
        "renderer/shaders/vert.glsl",
        "renderer/shaders/frag.glsl"
    );

    if (!postShader) {
        std::cerr << "Post shader failed\n";
        return -1;
    }

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    InitFramebuffer(w, h);
    InitScene();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        Render(window);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
