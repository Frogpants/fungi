#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D modelTexture;

void main() {
    vec3 color = vec3(abs(sin(vUV.x * 2.0) * cos(vUV.y)));
    FragColor = vec4(color, 1.0);
}
