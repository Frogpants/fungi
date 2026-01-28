#version 330 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform vec2 resolution;
uniform float time;

void main() {
    vec3 color = texture(screenTexture, uv).rgb;

    // Example: subtle vignette
    vec2 p = uv * 2.0 - 1.0;
    float vignette = 1.0 - dot(p, p) * 0.3;

    FragColor = vec4(color * vignette, 1.0);
}
