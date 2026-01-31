#pragma once

#include "../core/essentials.hpp"

inline vec3 camera = vec3(0.0);
inline vec3 camRot = vec3(0.0);

inline void moveXZ(float speed) {
    camera.xz = vec2(sin(camRot.x), cos(camRot.x)) * speed;
}