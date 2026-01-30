#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "./core/essentials.hpp"

#include "./renderer/projection.hpp"
#include "./renderer/camera.hpp"
#include "./game/models.hpp"

std::string LoadFile(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint CompileShader(const char* path, GLenum type) {
    std::string src = LoadFile(path);
    const char* csrc = src.c_str();

    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &csrc, nullptr);
    glCompileShader(s);

    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(s, 1024, nullptr, log);
        std::cerr << log << std::endl;
    }
    return s;
}

GLuint CreateShaderProgram(const char* vs, const char* fs) {
    GLuint v = CompileShader(vs, GL_VERTEX_SHADER);
    GLuint f = CompileShader(fs, GL_FRAGMENT_SHADER);

    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);

    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}

GLuint sceneFBO, sceneTexture, fullscreenVAO, postShader;

std::vector<Triangle> scene;
std::vector<Triangle> screenScene;

void InitScene() {
    Model shotgun;
    LoadOBJ(
        "game/models/shotgun.obj",
        shotgun,
        vec3(0, -0.25f, 5.0f), vec3(3)
    );

    AppendModelToScene(shotgun, scene);
}

void InitFramebuffer(int w, int h) {
    glGenFramebuffers(1, &sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

    glGenTextures(1, &sceneTexture);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, sceneTexture, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenVertexArrays(1, &fullscreenVAO);
}

inline vec2 ToNDC(vec2 p, int w, int h) {
    return {
        (p.x / w) * 2.0f - 1.0f,
        1.0f - (p.y / h) * 2.0f
    };
}

GLuint vao, vbo, shader;

void InitRender() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
}

void UploadTriangles(const std::vector<Triangle>& tris, int w, int h) {
    std::vector<float> verts;
    for (const Triangle& t : screenScene) {
        auto a = project(t.a.pos);
        auto b = project(t.b.pos);
        auto c = project(t.c.pos);

        a.x += w * 0.5f; a.y = h * 0.5f - a.y;
        b.x += w * 0.5f; b.y = h * 0.5f - b.y;
        c.x += w * 0.5f; c.y = h * 0.5f - c.y;

        verts.insert(verts.end(), {
            a.x, a.y, t.a.uv.x, t.a.uv.y,
            b.x, b.y, t.b.uv.x, t.b.uv.y,
            c.x, c.y, t.c.uv.x, t.c.uv.y
        });
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);
}

void DrawTriangles() {
    glUseProgram(shader);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, screenScene.size() * 3);
}

void Render(GLFWwindow* window) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    UpdVals(w);

    screenScene.clear();
    screenScene.reserve(scene.size());

    // Prepare vertex buffer with projected positions and UVs
    std::vector<float> verts;
    verts.reserve(scene.size() * 3 * 4); // 3 vertices per triangle, 4 floats per vertex (x,y,u,v)

    for (const Triangle& t : scene) {
        // Project 3D positions to 2D screen space
        vec2 a = project(t.a.pos);
        vec2 b = project(t.b.pos);
        vec2 c = project(t.c.pos);

        // Optionally skip triangles behind camera
        // if (a.x > 1e5f || b.x > 1e5f || c.x > 1e5f) continue;

        // Convert to screen coordinates
        a.x += w * 0.5f; a.y = h * 0.5f - a.y;
        b.x += w * 0.5f; b.y = h * 0.5f - b.y;
        c.x += w * 0.5f; c.y = h * 0.5f - c.y;

        // Push each vertex: x, y, u, v
        verts.insert(verts.end(), {
            a.x, a.y, t.a.uv.x, t.a.uv.y,
            b.x, b.y, t.b.uv.x, t.b.uv.y,
            c.x, c.y, t.c.uv.x, t.c.uv.y
        });
    }

    // Upload to GPU
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);

    // Clear screen
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw triangles
    DrawTriangles();
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Mama's Bakeria", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    camera = vec3(0, 0, 0);
    camRot = vec3(0);

    postShader = CreateShaderProgram(
        "renderer/shaders/vert.glsl",
        "renderer/shaders/frag.glsl"
    );

    InitRender();

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    InitFramebuffer(w, h);
    InitScene();

    GLuint texture;
    int tw, th, channels;
    unsigned char* data = stbi_load("game/models/shotgun-texture.png", &tw, &th, &channels, 4);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader, "modelTexture"), 0);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        Render(window);
        glfwSwapBuffers(window);
    }
}
