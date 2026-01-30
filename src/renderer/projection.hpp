#pragma once

#include "../core/essentials.hpp"
#include "camera.hpp"
#include <cmath>

inline float xCos = 0.0;
inline float xSin = 0.0;
inline float yCos = 0.0;
inline float ySin = 0.0;

inline float near = 0.1;

inline float FOV = 90.0;
inline float f = 960.0;

inline void UpdVals(int w) {
    xCos = cos(camRot.x);
    xSin = sin(camRot.x);
    yCos = cos(camRot.y);
    ySin = sin(camRot.y);

    f = (w / 2.0) / tan((FOV * 0.5f) * (3.14159f / 180.0f));
}

inline vec3 rotatePointLocal(vec3 pos, vec3 rp, vec3 angle) {
    vec3 p = pos - rp;
    p = vec3(p.x * cos(angle.x) - p.z * sin(angle.x), p.y, p.x * sin(angle.x) + p.z * cos(angle.x));
    p = vec3(p.x, p.y * cos(angle.y) - p.z * sin(angle.y), p.y * sin(angle.y) + p.z * cos(angle.y));
    p = vec3(p.x * cos(angle.z) - p.y * sin(angle.z), p.x * sin(angle.z) + p.y * cos(angle.z), p.z);

    return p;
}

inline vec3 rotatePointGlobal(vec3 pos, vec3 rp, vec3 angle) {
    vec3 p = pos - rp;
    vec3 point = vec3(p.x * cos(angle.x) - p.z * sin(angle.x), p.y, p.x * sin(angle.x) + p.z * cos(angle.x));
    point = vec3(p.x, p.y * cos(angle.y) - p.z * sin(angle.y), p.y * sin(angle.y) + p.z * cos(angle.y));
    point = vec3(p.x * cos(angle.z) - p.y * sin(angle.z), p.x * sin(angle.z) + p.y * cos(angle.z), p.z);

    return point;
}

inline vec2 project(vec3 pos) {
    vec3 p = pos - camera;

    float x = p.x;
    float y = p.y * cos(camRot.x) - p.z * sin(camRot.x);
    float z = p.y * sin(camRot.x) + p.z * cos(camRot.x);
    p.x = x; p.y = y; p.z = z;

    float x2 = p.x * cos(camRot.y) + p.z * sin(camRot.y);
    float z2 = -p.x * sin(camRot.y) + p.z * cos(camRot.y);
    p.x = x2; p.z = z2;

    float x3 = p.x * cos(camRot.z) - p.y * sin(camRot.z);
    float y3 = p.x * sin(camRot.z) + p.y * cos(camRot.z);
    p.x = x3; p.y = y3;

    if (p.z <= near) return vec2(1e6,1e6);

    return vec2(f*(p.x/p.z), f*(p.y/p.z));
}


