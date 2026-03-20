#pragma once

#include <glad/glad.h>

#include "triangle_mesh.hpp"

inline void DrawBasicFrame(GLuint program, const TriangleMesh& mesh) {
    glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glBindVertexArray(mesh.vao);
    glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
}
