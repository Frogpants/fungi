#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in float inTexBlend;

out vec3 vColor;
out vec2 vUV;
out float vTexBlend;
out vec3 vWorldPos;

uniform vec3 uCamPos;
uniform vec2 uCamRot;
uniform float uFovDeg;
uniform float uAspect;
uniform float uNear;
uniform float uFar;

void main() {
    vec3 p = inPos - uCamPos;

    // Inverse camera yaw (around Y)
    float yaw = -uCamRot.x;
    float cy = cos(yaw);
    float sy = sin(yaw);
    p = vec3(
        p.x * cy - p.z * sy,
        p.y,
        p.x * sy + p.z * cy
    );

    // Inverse camera pitch (around X)
    float pitch = -uCamRot.y;
    float cp = cos(pitch);
    float sp = sin(pitch);
    p = vec3(
        p.x,
        p.y * cp - p.z * sp,
        p.y * sp + p.z * cp
    );

    float z = p.z;

    float f = 1.0 / tan(radians(uFovDeg) * 0.5);
    float xClip = (p.x * f) / uAspect;
    float yClip = p.y * f;

    // Perspective depth mapping with +Z forward in view space.
    float zClip = ((uFar + uNear) / (uFar - uNear)) * z - ((2.0 * uFar * uNear) / (uFar - uNear));

    gl_Position = vec4(xClip, yClip, zClip, z);
    vColor = inColor;
    vUV = inUV;
    vTexBlend = inTexBlend;
    vWorldPos = inPos;
}