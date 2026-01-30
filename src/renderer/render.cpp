#include <glad/glad.h>
#include "render.hpp"

static vec2 ToNDC(const vec2& p, int width, int height) {
    return { (p.x / width) * 2.0f - 1.0f, 1.0f - (p.y / height) * 2.0f };
}

void RenderFrame(const std::vector<Triangle>& tris, int width, int height) {
    for (const Triangle& t : tris) {
        // Build vec2 positions from Vertex.pos
        vec2 pa(t.a.pos.x, t.a.pos.y);
        vec2 pb(t.b.pos.x, t.b.pos.y);
        vec2 pc(t.c.pos.x, t.c.pos.y);

        vec2 p1 = ToNDC(pa, width, height);
        vec2 p2 = ToNDC(pb, width, height);
        vec2 p3 = ToNDC(pc, width, height);

        // Now you can use p1, p2, p3 for your CPU-side rasterization
    }
}
