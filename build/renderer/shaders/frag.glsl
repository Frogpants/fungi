#version 330 core

out vec4 FragColor;
in vec2 vUV;

uniform sampler2D screenTexture;
uniform float time;
uniform vec2 resolution;

const float gamma = 2.2;


vec2 crtCurve(vec2 uv) {
    uv = uv * 2.0 - 1.0;

    float curve = 0.12;
    uv *= 1.0 + curve * uv * uv;

    uv = uv * 0.5 + 0.5;
    return uv;
}

float scanline(vec2 uv) {
    float scan = sin((uv.y - time*0.02) * resolution.y * 1.5);
    return 0.85 + 0.15 * scan;
}

vec3 shadowMask(vec2 uv) {
    float mask = mod(floor(uv.x * resolution.x), 0.1);
    vec3 rgb = vec3(0.9);
    if (mask == 0.0) rgb.r = 1.1;
    if (mask == 1.0) rgb.g = 1.1;
    if (mask == 2.0) rgb.b = 1.1;
    return rgb;
}


void main() {
    vec2 curvedUV = crtCurve(vUV);

    // if (curvedUV.x < 0.0 || curvedUV.x > 1.0 ||
    //     curvedUV.y < 0.0 || curvedUV.y > 1.0) {
    //     FragColor = vec4(0.0);
    //     return;
    // }

    vec3 color = texture(screenTexture, curvedUV).rgb;

    vec2 uv = vUV * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;
    color *= 0.95 + 0.05 * cos(uv.x * 3.0 + time);

    color *= scanline(curvedUV);

    // color *= shadowMask(curvedUV);

    color = pow(color, vec3(1.0 / gamma));

    float vignette = smoothstep(0.9, 0.4, length(uv*0.4));
    color *= vignette;

    FragColor = vec4(color, 1.0);
}