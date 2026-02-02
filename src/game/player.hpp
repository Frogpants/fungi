#pragma once

#include "../core/essentials.hpp"
#include "./manager.hpp"

#include <cmath>


struct Player {
    vec3 pos = vec3(0.0);
    vec3 rot = vec3(0.0);
};

inline Player player;

void move(float speed) {
    player.pos.x += speed * std::sin(player.rot.x);
    player.pos.z += speed * std::cos(player.rot.x);
}


void controls() {



}


namespace Carl {

    inline float speak = 0.0;

    inline void Init() {
        // say hi
    }

    inline void Update() {
        // say hi again
    }
}



// refrence! :) 
float math(float a, float b) {
    return a + b;
}


vec2 math(vec2 a, vec2 b) {
    vec2 v = a + b;
    return v;
}


float v1 = 1.0;
float v2 = 2.0;

float v = math(v1, v2);


vec2 v1 = vec2(1.0);
vec2 v2 = vec2(2.0);

vec2 v = math(v1, v2);