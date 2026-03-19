#include <glad/glad.h>
#include <GLFW/glfw3.h>
#if __has_include(<stb_image.h>)
#include <stb_image.h>
#elif __has_include(<stb/stb_image.h>)
#include <stb/stb_image.h>
#else
#error "stb_image header not found"
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <cstdio>
#include <filesystem>

#include "./core/essentials.hpp"
#include "./renderer/camera.hpp"
#include "./game/models.hpp"

#include "./game/manager.hpp"

static GLuint vao, vbo, shader, texture;
static Mesh scene;

struct FpsQualityPreset {
    const char* name;
    int targetFps;
    int vsync;
    GLint minFilter;
    GLint magFilter;
};

static constexpr FpsQualityPreset kQualityPresets[] = {
    {"Performance", 240, 0, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST},
    {"Balanced", 120, 1, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR},
    {"Quality", 60, 1, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR}
};

static int g_qualityIndex = 1;
static float g_fov = 90.0f;
static float g_near = 0.1f;
static float g_far = 1000.0f;
static std::filesystem::path g_exeDir;

bool IsReadableFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    return file.is_open();
}

std::string ResolveAssetPath(const std::string& relativePath) {
    const std::string exeDir = g_exeDir.string();
    const std::string candidates[] = {
        relativePath,
        std::string("dist/") + relativePath,
        exeDir.empty() ? std::string() : (exeDir + "/" + relativePath),
        exeDir.empty() ? std::string() : (exeDir + "/../" + relativePath)
    };

    for (const auto& candidate : candidates) {
        if (!candidate.empty() && IsReadableFile(candidate)) {
            return candidate;
        }
    }

    throw std::runtime_error("Failed to open file: " + relativePath);
}

std::string LoadFile(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error(std::string("Failed to open file: ") + path);
    }
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
        std::string typeName = (type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
        std::cerr << "Shader compile error (" << typeName << ") in " << path << "\n" << log << std::endl;
        glDeleteShader(s);
        throw std::runtime_error("Shader compile failed");
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

    GLint ok = GL_FALSE;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(p, 1024, nullptr, log);
        std::cerr << "Shader link error\n" << log << std::endl;
        glDeleteProgram(p);
        glDeleteShader(v);
        glDeleteShader(f);
        throw std::runtime_error("Shader link failed");
    }

    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}

void InitScene() {
    Model shotgun;
    const std::string shotgunObjPath = ResolveAssetPath("game/models/shotgun.obj");
    if (!LoadOBJ(
        shotgunObjPath,
        shotgun,
        vec3(0, 0, 5.0f),
        vec3(3.0f)
    )) {
        std::cerr << "Model load failed." << std::endl;
    }

    scene.Append(shotgun);

    // Example manual geometry path: same mesh API as loaded models.
    scene.AddTriangle(
        vec3(-1.0f, -1.0f, 2.5f), vec2(0.0f, 0.0f),
        vec3( 1.0f, -1.0f, 2.5f), vec2(1.0f, 0.0f),
        vec3( 0.0f,  0.8f, 2.5f), vec2(0.5f, 1.0f)
    );
}

void InitRender() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        5 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE,
        5 * sizeof(float),
        (void*)(3 * sizeof(float))
    );
}

void UploadTriangles() {
    std::vector<float> verts;
    verts.reserve(scene.TriangleCount() * 3 * 5);

    for (const Triangle& t : scene.triangles) {
        verts.insert(verts.end(), {
            t.a.pos.x, t.a.pos.y, t.a.pos.z, t.a.uv.x, t.a.uv.y,
            t.b.pos.x, t.b.pos.y, t.b.pos.z, t.b.uv.x, t.b.uv.y,
            t.c.pos.x, t.c.pos.y, t.c.pos.z, t.c.uv.x, t.c.uv.y
        });
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        verts.size() * sizeof(float),
        verts.data(),
        GL_STATIC_DRAW
    );
}

void ApplyFpsQuality(GLFWwindow* window) {
    const FpsQualityPreset& preset = kQualityPresets[g_qualityIndex];
    glfwSwapInterval(preset.vsync);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, preset.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, preset.magFilter);

    if (preset.minFilter == GL_LINEAR_MIPMAP_LINEAR) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    if (window) {
        std::cout << "FPS quality set to: " << preset.name << " (target " << preset.targetFps << " FPS)" << std::endl;
    }
}


void Controls(GLFWwindow* window) {
    const float moveSpeed = 4.0f * deltaTime;

    if (Input::IsDown("w")) {
        moveForward(moveSpeed);
    }
    if (Input::IsDown("s")) {
        moveForward(-moveSpeed);
    }

    if (Input::IsDown("d")) {
        moveRight(moveSpeed);
    }
    if (Input::IsDown("a")) {
        moveRight(-moveSpeed);
    }

    if (Input::IsDown("e")) {
        camera.pos.y += moveSpeed;
    }
    if (Input::IsDown("q")) {
        camera.pos.y -= moveSpeed;
    }

    if (Mouse::IsPressed(0) && !Mouse::IsLocked()) {
        Mouse::Lock();
    }
    if (Mouse::IsLocked()) {
        camera.rot.x += static_cast<float>(Mouse::DeltaX()) * -0.003f;
        camera.rot.y -= static_cast<float>(Mouse::DeltaY()) * -0.003f;

        if (camera.rot.y > 1.55f) camera.rot.y = 1.55f;
        if (camera.rot.y < -1.55f) camera.rot.y = -1.55f;

        if (Input::IsPressed("escape")) {
            Mouse::Unlock();
        }
    }

    if (Input::IsPressed("1")) {
        g_qualityIndex = 0;
        ApplyFpsQuality(window);
    }
    if (Input::IsPressed("2")) {
        g_qualityIndex = 1;
        ApplyFpsQuality(window);
    }
    if (Input::IsPressed("3")) {
        g_qualityIndex = 2;
        ApplyFpsQuality(window);
    }
}


void Render(GLFWwindow* window) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    glViewport(0, 0, w, h);

    glClearColor(0.04f, 0.04f, 0.07f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader, "modelTexture"), 0);
    glUniform3f(
        glGetUniformLocation(shader, "uCamPos"),
        camera.pos.x, camera.pos.y, camera.pos.z
    );
    glUniform2f(
        glGetUniformLocation(shader, "uCamRot"),
        camera.rot.x, camera.rot.y
    );
    glUniform1f(glGetUniformLocation(shader, "uFovDeg"), g_fov);
    glUniform1f(glGetUniformLocation(shader, "uAspect"), static_cast<float>(w) / static_cast<float>(h));
    glUniform1f(glGetUniformLocation(shader, "uNear"), g_near);
    glUniform1f(glGetUniformLocation(shader, "uFar"), g_far);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, scene.TriangleCount() * 3);
}

int main(int argc, char** argv) {
    if (argc > 0 && argv != nullptr && argv[0] != nullptr) {
        g_exeDir = std::filesystem::path(argv[0]).parent_path();
    }

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1280, 720, "Mama's Bakeria", nullptr, nullptr
    );
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwFocusWindow(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    camera.pos = vec3(0.0);
    camera.rot = vec3(0.0);

    const std::string vertShaderPath = ResolveAssetPath("renderer/shaders/vert.glsl");
    const std::string fragShaderPath = ResolveAssetPath("renderer/shaders/frag.glsl");

    try {
        shader = CreateShaderProgram(
            vertShaderPath.c_str(),
            fragShaderPath.c_str()
        );
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    InitRender();
    InitScene();
    UploadTriangles();

    int w, h, ch;
    const std::string texturePath = ResolveAssetPath("game/models/shotgun-texture.png");
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(
        texturePath.c_str(),
        &w, &h, &ch, 4
    );
    if (!data) {
        std::cerr << "Failed to load texture: " << texturePath << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        w, h, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    ApplyFpsQuality(window);

    Manager::Init(window);

    double lastFrameTime = glfwGetTime();
    double fpsAccum = 0.0;
    int fpsFrames = 0;

    while (!glfwWindowShouldClose(window)) {
        const double frameStart = glfwGetTime();
        deltaTime = static_cast<float>(frameStart - lastFrameTime);
        lastFrameTime = frameStart;

        if (deltaTime < 0.0f) {
            deltaTime = 0.0f;
        }
        if (deltaTime > 0.25f) {
            deltaTime = 0.25f;
        }

        glfwPollEvents();
        timer += deltaTime;

        Manager::Update();

        Controls(window);

        Render(window);
        glfwSwapBuffers(window);

        fpsAccum += deltaTime;
        fpsFrames += 1;

        if (fpsAccum >= 0.5) {
            const double fps = static_cast<double>(fpsFrames) / fpsAccum;
            char title[256];
            std::snprintf(
                title,
                sizeof(title),
                "Fungi | FPS: %.1f | Quality: %s (1/2/3)",
                fps,
                kQualityPresets[g_qualityIndex].name
            );
            glfwSetWindowTitle(window, title);
            fpsAccum = 0.0;
            fpsFrames = 0;
        }

        const double frameEnd = glfwGetTime();
        const double frameDuration = frameEnd - frameStart;
        const double targetFrame = 1.0 / static_cast<double>(kQualityPresets[g_qualityIndex].targetFps);

        if (frameDuration < targetFrame) {
            std::this_thread::sleep_for(
                std::chrono::duration<double>(targetFrame - frameDuration)
            );
        }
    }

    glDeleteTextures(1, &texture);
    glDeleteProgram(shader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
