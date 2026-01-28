#include "../core/essentials.hpp"
#include "camera.hpp"
#include <cmath>

float xCos = 0.0;
float xSin = 0.0;
float yCos = 0.0;
float ySin = 0.0;

float near = 0.1;

float FOV = 90.0;
float f = 0.0;

void UpdVals() {
    xCos = cos(camRot.x);
    xSin = sin(camRot.x);
    yCos = cos(camRot.y);
    ySin = sin(camRot.y);

    f = 1.0f / tan((FOV * 0.5f) * (3.14159f / 180.0f));
}

vec2 project(vec3 pos) {
    vec3 p = pos - camera;
    p = vec3(p.x * xCos - p.z * xSin, p.y, p.x * xSin + p.z * xCos);
    p = vec3(p.y, p.y * yCos - p.z * ySin, p.y * ySin + p.z * yCos);
    p = min(p, near);
    return vec2(f*(p.x/p.z), f*(p.y/p.z));
}


