#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D modelTexture;

void main() {
    vec3 color = vec3(vUV.x,vUV.y,abs(sin(vUV.x + vUV.y)));
    FragColor = vec4(color, 1.0);
}
