#version 330 core

out vec4 FragColor;
in vec2 vUV;

uniform sampler2D screenTexture;
uniform float time;
uniform vec2 resolution;

void main() {
    vec2 uv = vUV;

    uv -= 0.5;
    uv *= 1.0 + 0.02 * length(uv);
    uv += 0.5;

    vec3 color = texture(screenTexture, uv).rgb * vec3(uv*cos(time*0.5), sin(time));

    // Slight vignette
    float vignette = smoothstep(0.9, 0.4, length(vUV - 0.5));
    color *= vignette;

    // Simple color grading
    color = pow(color, vec3(1.1)); // gamma
    color *= vec3(1.05, 1.0, 0.95); // warm tint

    FragColor = vec4(color, 1.0);
}
