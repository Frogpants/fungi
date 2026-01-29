#version 330 core

out vec4 FragColor;
in vec2 vUV;

uniform sampler2D screenTexture;
uniform float time;
uniform vec2 resolution;

uniform float gamemode;

const float gamma = 2.2;

vec2 crtCurve(vec2 uv) {
    uv = uv * 2.0 - 1.0;
    uv *= 1.0 + 0.15 * uv * uv;
    uv = uv * 0.5 + 0.5;
    return uv;
}

float scanline(vec2 uv) {
    return 0.9 + 0.1 * sin((uv.y * resolution.y + time * 30.0) * 1.2);
}

vec3 shadowMask(vec2 uv) {
    float m = mod(floor(uv.x * resolution.x), 3.0);
    return vec3(
        1.0 + 0.15 * step(m, 0.5),
        1.0 + 0.15 * step(abs(m - 1.0), 0.5),
        1.0 + 0.15 * step(abs(m - 2.0), 0.5)
    );
}

float noise(vec2 uv) {
    return fract(sin(dot(uv * resolution.xy + time, vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    vec2 curvedUV = crtCurve(vUV);
    vec2 pixelUV = floor(curvedUV * resolution * 0.5) / (resolution * 0.5);

    vec2 uv = curvedUV * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    vec3 color = texture(screenTexture, pixelUV).rgb;

    float dist = length(uv);
    float edge = smoothstep(1.6, 2.0, dist);
    vec3 glow = vec3(0.08, 0.05, 0.03) * edge;
    color = color * (1.0 - edge) + glow;

    color += texture(screenTexture, pixelUV + vec2(1.0 / resolution.x, 0.0)).rgb * 0.15;
    color -= texture(screenTexture, pixelUV - vec2(1.0 / resolution.x, 0.0)).rgb * 0.1;

    color *= scanline(curvedUV);
    color *= shadowMask(curvedUV);

    color *= 0.7 + 0.2 * noise(curvedUV);

    if (gamemode == 1.0) {
        float vignette = smoothstep(1.0, 0.35, length(uv*0.5));
        color *= vignette;
    }

    color = pow(color, vec3(1.0 / gamma));

    FragColor = vec4(color, 1.0);
}