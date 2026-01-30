// #version 330 core

// out vec2 vUV;

// const vec2 verts[3] = vec2[](
//     vec2(-1.0, -1.0),
//     vec2( 3.0, -1.0),
//     vec2(-1.0,  3.0)
// );

// void main() {
//     vec2 pos = verts[gl_VertexID];
//     vUV = pos * 0.5 + 0.5;
//     gl_Position = vec4(pos, 0.0, 1.0);
// }

#version 330 core
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inUV;

out vec2 vUV;

void main() {
    gl_Position = vec4(inPos, 0.0, 1.0);
    vUV = inUV;
}

