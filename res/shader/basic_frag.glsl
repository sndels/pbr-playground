#version 410

#include "hg_sdf.glsl"

uniform float uGT;
uniform vec2  uRes;

out vec4 fragColor;

// Camera
vec3  CAM_POS = vec3(0, 0, -3);
vec3  CAM_TARGET = CAM_POS + vec3(0, 0, 1);
vec3  CAM_UP = vec3(0, 1, 0);
float CAM_FOV = 65;

mat3 camOrient(vec3 eye, vec3 target, vec3 up)
{
    vec3 n = normalize(target - eye);
    vec3 u = normalize(cross(up, n));
    vec3 v = cross(n, u);
    return mat3(u, v, n);
}

vec3 getViewRay(vec2 fragCoord, vec2 resolution, float fov)
{
    vec2 xy = fragCoord - resolution * 0.5;
    float z = resolution.y / tan(radians(fov * 0.5));
    return normalize(vec3(xy, z));
}

void main()
{
    // Calculate view ray direction in scene space
    vec3 vr = getViewRay(gl_FragCoord.xy, uRes, CAM_FOV);
    vr = camOrient(CAM_POS, CAM_TARGET, CAM_UP) * vr;
    fragColor = vec4(vr, uGT);
}
