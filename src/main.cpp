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

GLuint vao, vbo, shader;
GLuint texture;

std::vector<Triangle> scene;

void InitScene() {
    Model shotgun;
    LoadOBJ("game/models/shotgun.obj", shotgun, vec3(0, -0.25f, 5.0f), vec3(3));
    scene.insert(scene.end(), shotgun.triangles.begin(), shotgun.triangles.end());
}

void InitRender() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Each vertex has 4 floats: x, y, u, v
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void UploadTriangles(const std::vector<Triangle>& tris, int w, int h) {
    std::vector<float> verts;
    verts.reserve(tris.size() * 3 * 4); // 3 vertices per triangle, 4 floats per vertex (x,y,u,v)

    auto toScreen = [&](vec3 p) -> vec2 {
        vec2 v = project(p);
        v.x += w * 0.5f;
        v.y = h * 0.5f - v.y;
        return v;
    };

    for (const Triangle& t : tris) {
        vec2 a = toScreen(t.a.pos);
        vec2 b = toScreen(t.b.pos);
        vec2 c = toScreen(t.c.pos);

        verts.push_back(a.x); verts.push_back(a.y); verts.push_back(t.a.uv.x); verts.push_back(t.a.uv.y);
        verts.push_back(b.x); verts.push_back(b.y); verts.push_back(t.b.uv.x); verts.push_back(t.b.uv.y);
        verts.push_back(c.x); verts.push_back(c.y); verts.push_back(t.c.uv.x); verts.push_back(t.c.uv.y);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);
}

void Render(GLFWwindow* window) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    UploadTriangles(scene, w, h);

    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader, "modelTexture"), 0);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, scene.size() * 3);
}

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Mama's Bakeria", nullptr, nullptr);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    camera = vec3(0, 0, 0);
    camRot = vec3(0);

    shader = CreateShaderProgram("renderer/shaders/vert.glsl", "renderer/shaders/frag.glsl");

    InitRender();
    InitScene();

    // Load texture
    int tw, th, channels;
    unsigned char* data = stbi_load("game/models/shotgun-texture.png", &tw, &th, &channels, 4);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        Render(window);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
