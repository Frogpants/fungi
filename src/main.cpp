#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer/frame_draw.hpp"
#include "renderer/shader_program.hpp"
#include "renderer/triangle_mesh.hpp"
#include "renderer/window_context.hpp"

int main() {
    WindowContext ctx;
    if (!InitWindowContext(ctx, 800, 600, "fungi")) {
        return 1;
    }

    ClearTrianglesList();
    Triangle(vec2(0.0f, 0.5f), vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f), 0.0f);
    const TriangleMesh mesh = CreateTriangleMeshFromList();

    const GLuint program = CreateBasicColorProgram();
    if (!program) {
        DestroyTriangleMesh(mesh);
        ShutdownWindowContext(ctx);
        return 1;
    }

    while (!glfwWindowShouldClose(ctx.window)) {
        glfwPollEvents();
        DrawBasicFrame(program, mesh);
        glfwSwapBuffers(ctx.window);
    }

    glDeleteProgram(program);
    DestroyTriangleMesh(mesh);
    ShutdownWindowContext(ctx);
    return 0;
}
