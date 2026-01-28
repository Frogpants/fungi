#pragma once
#include "../core/essentials.hpp"

struct BoxCollider {
    vec3 halfSize;
};

struct RigidBody {
    vec3 position;
    vec3 velocity;
    vec3 force;

    float mass;
    bool isStatic = false;
};

struct BoxBody {
    RigidBody body;
    BoxCollider collider;
};
