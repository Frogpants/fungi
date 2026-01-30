#include <glad/glad.h>
#include "render.hpp"

static vec2 ToNDC(const vec2& p, int width, int height) {
    return {
        (p.x / (float)width) * 2.0f - 1.0f,
        1.0f - (p.y / (float)height) * 2.0f
    };
}

void RenderFrame(const std::vector<Triangle>& tris, int width, int height) {
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // TEMP: compatibility mode check
    // This WILL draw if compatibility context exists
    glBegin(GL_LINES);
    for (const Triangle& t : tris) {
        vec2 p1 = ToNDC({t.a.x, t.a.y}, width, height);
        vec2 p2 = ToNDC({t.b.x, t.b.y}, width, height);
        vec2 p3 = ToNDC({t.c.x, t.c.y}, width, height);

        glVertex2f(p1.x, p1.y); glVertex2f(p2.x, p2.y);
        glVertex2f(p2.x, p2.y); glVertex2f(p3.x, p3.y);
        glVertex2f(p3.x, p3.y); glVertex2f(p1.x, p1.y);
    }
    glEnd();
}
