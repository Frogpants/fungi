#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <stdexcept>

#include "./core/essentials.hpp"
#include "./renderer/projection.hpp"
#include "./renderer/camera.hpp"
#include "./game/models.hpp"

#include "./game/manager.hpp"

static GLuint vao, vbo, shader, texture;
static std::vector<Triangle> scene;

std::string LoadFile(const char* path) {
    std::ifstream file(path);
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

void InitScene() {
    Model shotgun;
    LoadOBJ(
        "game/models/shotgun.obj",
        shotgun,
        vec3(0, 0, 5.0f),
        vec3(3.0f)
    );

    scene.insert(scene.end(),
        shotgun.triangles.begin(),
        shotgun.triangles.end()
    );
}

void InitRender() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof(float),
        (void*)(2 * sizeof(float))
    );
}

void UploadTriangles(int w, int h) {
    std::vector<float> verts;
    verts.reserve(scene.size() * 3 * 4);

    auto toScreen = [&](const vec3& p) {
        vec2 v = project(p);
        v.x = (v.x / (w * 0.5f));
        v.y = (v.y / (h * 0.5f));
        return v;
    };

    for (const Triangle& t : scene) {
        vec2 a = toScreen(t.a.pos);
        vec2 b = toScreen(t.b.pos);
        vec2 c = toScreen(t.c.pos);

        verts.insert(verts.end(), {
            a.x, a.y, t.a.uv.x, t.a.uv.y,
            b.x, b.y, t.b.uv.x, t.b.uv.y,
            c.x, c.y, t.c.uv.x, t.c.uv.y
        });
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        verts.size() * sizeof(float),
        verts.data(),
        GL_DYNAMIC_DRAW
    );
}


void Controls(int w, int h) {
    if (Input::IsDown("w")) {
        moveForward(0.1);
    }
    if (Input::IsDown("s")) {
        moveForward(-0.1);
    }

    if (Input::IsDown("d")) {
        moveRight(0.1);
    }
    if (Input::IsDown("a")) {
        moveRight(-0.1);
    }

    if (Input::IsDown("e")) {
        camera.pos.y += 0.1;
    }
    if (Input::IsDown("q")) {
        camera.pos.y -= 0.1;
    }

    // if (Mouse::IsPressed(0) && !Mouse::IsLocked()) {
    //     Mouse::Lock();
    // }
    // if (Mouse::IsLocked()) {
    //     camera.rot.x += Mouse::DeltaX() * 0.001;
    //     camera.rot.y -= Mouse::DeltaY() * 0.001;
    //     if (Input::IsPressed("esc")) {
    //         Mouse::Unlock();
    //     }
    // }
}


void Render(GLFWwindow* window) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    glViewport(0, 0, w, h);
    UpdVals(w);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    UploadTriangles(w, h);

    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader, "modelTexture"), 0);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, scene.size() * 3);
}

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1280, 720, "Mama's Bakeria", nullptr, nullptr
    );

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    camera.pos = vec3(0.0);
    camera.rot = vec3(0.0);

    shader = CreateShaderProgram(
        "renderer/shaders/vertTri.glsl",
        "renderer/shaders/fragTri.glsl"
    );

    InitRender();
    InitScene();

    int w, h, ch;
    unsigned char* data = stbi_load(
        "game/models/shotgun-texture.png",
        &w, &h, &ch, 4
    );

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        w, h, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    Manager::Init(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        timer += deltaTime;

        Controls(w, h);

        Manager::Update();

        Render(window);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
