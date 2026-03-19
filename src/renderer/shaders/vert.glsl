#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUV;

out vec2 vUV;

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
    if (z <= uNear) {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        vUV = inUV;
        return;
    }

    float f = 1.0 / tan(radians(uFovDeg) * 0.5);
    float xClip = (p.x * f) / uAspect;
    float yClip = p.y * f;
    float zNdc = ((z - uNear) / (uFar - uNear)) * 2.0 - 1.0;

    gl_Position = vec4(xClip, yClip, zNdc * z, z);
    vUV = inUV;
}