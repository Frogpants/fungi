#pragma once

#include <glad/glad.h>
#include <vector>

#include "../core/essentials.hpp"

struct Triangle;

inline std::vector<Triangle>& GetTrianglesList();
inline void ClearTrianglesList();

struct Vertex {
    vec3 pos = vec3(0.0f, 0.0f, 0.0f);
    vec3 color = vec3(1.0f, 1.0f, 1.0f);

    Vertex() = default;

    Vertex(float px, float py, float pz, float pr, float pg, float pb)
        : pos(px, py, pz), color(pr, pg, pb) {}

    Vertex(const vec3& p, const vec3& c)
        : pos(p), color(c) {}

    Vertex(const vec2& p, const vec3& c)
        : pos(p.x, p.y, 0.0f), color(c) {}

    Vertex(const vec2& p, float pr, float pg, float pb)
        : pos(p.x, p.y, 0.0f), color(pr, pg, pb) {}
};

struct Triangle {
    Vertex v1;
    Vertex v2;
    Vertex v3;
    float tex = 0.0f;

    Triangle()
        : Triangle(Vertex(), Vertex(), Vertex(), 0.0f, true) {}

    Triangle(const Vertex& a, const Vertex& b, const Vertex& c, float texture = 0.0f)
        : Triangle(a, b, c, texture, true) {}

    Triangle(const vec2& p1, const vec2& p2, const vec2& p3, float texture = 0.0f)
        : Triangle(
              Vertex(p1, vec3(1.0f, 0.0f, 0.0f)),
              Vertex(p2, vec3(0.0f, 1.0f, 0.0f)),
              Vertex(p3, vec3(0.0f, 0.0f, 1.0f)),
              texture,
              true
          ) {}

private:
    Triangle(const Vertex& a, const Vertex& b, const Vertex& c, float texture, bool registerInList)
        : v1(a), v2(b), v3(c), tex(texture) {
        if (registerInList) {
            GetTrianglesList().emplace_back(v1, v2, v3, tex, false);
        }
    }

    friend std::vector<Triangle>& GetTrianglesList();
};

inline std::vector<Triangle>& GetTrianglesList() {
    static std::vector<Triangle> triangles;
    return triangles;
}

inline void ClearTrianglesList() {
    GetTrianglesList().clear();
}

struct TriangleMesh {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLsizei vertexCount = 0;
};

inline TriangleMesh CreateTriangleMesh(const std::vector<Triangle>& triangles) {
    std::vector<float> vertices;
    vertices.reserve(triangles.size() * 3 * 6);

    for (const Triangle& t : triangles) {
        vertices.insert(vertices.end(), {t.v1.pos.x, t.v1.pos.y, t.v1.pos.z, t.v1.color.x, t.v1.color.y, t.v1.color.z});
        vertices.insert(vertices.end(), {t.v2.pos.x, t.v2.pos.y, t.v2.pos.z, t.v2.color.x, t.v2.color.y, t.v2.color.z});
        vertices.insert(vertices.end(), {t.v3.pos.x, t.v3.pos.y, t.v3.pos.z, t.v3.color.x, t.v3.color.y, t.v3.color.z});
    }

    TriangleMesh mesh;
    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);

    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    mesh.vertexCount = static_cast<GLsizei>(triangles.size() * 3);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return mesh;
}

inline TriangleMesh CreateTriangleMeshFromList() {
    return CreateTriangleMesh(GetTrianglesList());
}

inline void DestroyTriangleMesh(const TriangleMesh& mesh) {
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteVertexArrays(1, &mesh.vao);
}
