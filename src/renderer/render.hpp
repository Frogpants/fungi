#pragma once
#include <vector>
#include "../core/essentials.hpp"

struct Vertex {
    vec3 pos;
    vec2 uv;
};

struct Triangle {
    Vertex a, b, c;
};

void RenderFrame(const std::vector<Triangle>& tris, int width, int height);
