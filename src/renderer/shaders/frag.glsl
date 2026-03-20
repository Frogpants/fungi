#version 330 core

in vec3 vColor;
in vec2 vUV;
in float vTexBlend;
in vec3 vWorldPos;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec3 uViewPos;
uniform vec3 uSunDir;
uniform vec3 uSunColor;
uniform vec3 uPointPos;
uniform vec3 uPointColor;
uniform float uAmbientStrength;

void main() {
    vec3 texColor = texture(uTexture, vUV).rgb;
    vec3 baseColor = mix(vColor, texColor, clamp(vTexBlend, 0.0, 1.0));

    vec3 normal = normalize(cross(dFdx(vWorldPos), dFdy(vWorldPos)));
    if (!gl_FrontFacing) {
        normal = -normal;
    }

    vec3 Ld = normalize(uSunDir);
    float diffSun = max(dot(normal, Ld), 0.0);

    vec3 pointVec = uPointPos - vWorldPos;
    float dist = length(pointVec);
    vec3 Lp = (dist > 0.0001) ? (pointVec / dist) : vec3(0.0, 1.0, 0.0);
    float attenuation = 1.0 / (1.0 + 0.12 * dist + 0.03 * dist * dist);
    float diffPoint = max(dot(normal, Lp), 0.0) * attenuation;

    vec3 viewDir = normalize(uViewPos - vWorldPos);
    vec3 halfDir = normalize(Ld + viewDir);
    float specSun = pow(max(dot(normal, halfDir), 0.0), 32.0) * 0.18;

    vec3 lit = baseColor * uAmbientStrength;
    lit += baseColor * uSunColor * diffSun;
    lit += baseColor * uPointColor * diffPoint;
    lit += vec3(1.0, 0.95, 0.85) * specSun;

    // Mild gamma correction for less flat output.
    lit = pow(max(lit, 0.0), vec3(1.0 / 2.2));
    FragColor = vec4(lit, 1.0);
}