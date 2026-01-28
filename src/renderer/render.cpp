#include <glad/glad.h>
#include "render.hpp"
#include "projection.hpp"

static vec2 ToNDC(vec2 p, int width, int height) {
    return {
        (p.x / width) * 2.0f - 1.0f,
        1.0f - (p.y / height) * 2.0f
    };
}

void RenderFrame(const std::vector<Triangle>& tris, int width, int height) {
    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_LINES);
    for (const Triangle& t : tris) {
        vec2 p1 = project(t.a);
        vec2 p2 = project(t.b);
        vec2 p3 = project(t.c);

        p1 = ToNDC(p1, width, height);
        p2 = ToNDC(p2, width, height);
        p3 = ToNDC(p3, width, height);

        glVertex2f(p1.x, p1.y); glVertex2f(p2.x, p2.y);
        glVertex2f(p2.x, p2.y); glVertex2f(p3.x, p3.y);
        glVertex2f(p3.x, p3.y); glVertex2f(p1.x, p1.y);
    }
    glEnd();
}
