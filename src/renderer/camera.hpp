#pragma once

#include "../core/essentials.hpp"

struct Camera {
    vec3 pos = vec3(0.0);
    vec3 rot = vec3(0.0);
};

inline Camera camera;

inline void moveForward(float speed) {
    camera.pos.x += sin(camera.rot.x) * speed;
    camera.pos.z += cos(camera.rot.x) * speed;
}

inline void moveRight(float speed) {
    camera.pos.x += cos(camera.rot.x) * speed;
    camera.pos.z -= sin(camera.rot.x) * speed;
}
