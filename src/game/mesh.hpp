#pragma once

#include <vector>
#include "../core/essentials.hpp"

struct Vertex {
    vec3 pos;
    vec2 uv;
};

struct Triangle {
    Vertex a;
    Vertex b;
    Vertex c;
};

inline Vertex MakeVertex(const vec3& pos, const vec2& uv = vec2(0.0f, 0.0f)) {
    return Vertex{pos, uv};
}

inline Triangle MakeTriangle(const Vertex& a, const Vertex& b, const Vertex& c) {
    return Triangle{a, b, c};
}

inline Triangle MakeTriangle(
    const vec3& aPos, const vec2& aUv,
    const vec3& bPos, const vec2& bUv,
    const vec3& cPos, const vec2& cUv
) {
    return Triangle{
        MakeVertex(aPos, aUv),
        MakeVertex(bPos, bUv),
        MakeVertex(cPos, cUv)
    };
}

struct Mesh {
    std::vector<Triangle> triangles;

    void AddTriangle(const Triangle& t) {
        triangles.push_back(t);
    }

    void AddTriangle(const Vertex& a, const Vertex& b, const Vertex& c) {
        triangles.push_back(MakeTriangle(a, b, c));
    }

    void AddTriangle(
        const vec3& aPos, const vec2& aUv,
        const vec3& bPos, const vec2& bUv,
        const vec3& cPos, const vec2& cUv
    ) {
        triangles.push_back(MakeTriangle(aPos, aUv, bPos, bUv, cPos, cUv));
    }

    void Append(const Mesh& other) {
        triangles.insert(triangles.end(), other.triangles.begin(), other.triangles.end());
    }

    void Clear() {
        triangles.clear();
    }

    bool Empty() const {
        return triangles.empty();
    }

    size_t TriangleCount() const {
        return triangles.size();
    }
};
