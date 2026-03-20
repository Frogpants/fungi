#pragma once

#include <vector>
#include "../core/essentials.hpp"

struct Vertex {
    vec3 pos;
    vec2 uv;
    vec3 color = vec3(1.0f, 1.0f, 1.0f);
    float texBlend = 0.0f;
};

struct Triangle {
    Vertex a;
    Vertex b;
    Vertex c;
};

inline Vertex MakeVertex(
    const vec3& pos,
    const vec2& uv = vec2(0.0f, 0.0f),
    const vec3& color = vec3(1.0f, 1.0f, 1.0f),
    float texBlend = 0.0f
) {
    return Vertex{pos, uv, color, texBlend};
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
        MakeVertex(aPos, aUv, vec3(1.0f, 1.0f, 1.0f), 1.0f),
        MakeVertex(bPos, bUv, vec3(1.0f, 1.0f, 1.0f), 1.0f),
        MakeVertex(cPos, cUv, vec3(1.0f, 1.0f, 1.0f), 1.0f)
    };
}

inline Triangle MakeTriangle(
    const vec3& aPos, const vec3& aColor,
    const vec3& bPos, const vec3& bColor,
    const vec3& cPos, const vec3& cColor
) {
    return Triangle{
        MakeVertex(aPos, vec2(0.0f, 0.0f), aColor, 0.0f),
        MakeVertex(bPos, vec2(0.0f, 0.0f), bColor, 0.0f),
        MakeVertex(cPos, vec2(0.0f, 0.0f), cColor, 0.0f)
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

    void AddTriangle(
        const vec3& aPos, const vec3& aColor,
        const vec3& bPos, const vec3& bColor,
        const vec3& cPos, const vec3& cColor
    ) {
        triangles.push_back(MakeTriangle(aPos, aColor, bPos, bColor, cPos, cColor));
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
