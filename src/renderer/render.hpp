#pragma once
#include <vector>
#include "../core/essentials.hpp"

struct Triangle {
    vec3 a;
    vec3 b;
    vec3 c;
};

void RenderFrame(const std::vector<Triangle>& tris, int width, int height);
