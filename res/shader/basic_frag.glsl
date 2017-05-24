#version 410

#include "hg_sdf.glsl"
#include "uniforms.glsl"

out vec4 fragColor;

// Ray marching
const int   MAX_MARCHING_STEPS = 256;
const float MIN_DIST = 0;
const float MAX_DIST = 10;
const float EPSILON = 0.0001;

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

float fScene(vec3 p)
{
    vec3 spherePos = vec3(0);
    return fSphere(p - spherePos, 0.75);
}

float castRay(vec3 rd, vec3 ro)
{
    float depth = MIN_DIST;
    for (int i = 0; i < MAX_MARCHING_STEPS; ++i) {
        float dist = fScene(ro + depth * rd);
        if (dist < EPSILON) break;

        depth += dist;

        if (depth > MAX_DIST) break;
    }
    return depth;
}

void main()
{
    // Calculate view ray direction in scene space
    vec3 rd = getViewRay(gl_FragCoord.xy, uRes, CAM_FOV);
    rd = camOrient(CAM_POS, CAM_TARGET, CAM_UP) * rd;

    // Cast a ray into scene
    float depth = castRay(rd, CAM_POS);

    // Check if it missed
    if (depth > MAX_DIST - EPSILON) {
        fragColor = vec4(0);
        return;
    }

    fragColor = vec4(rd, 1);
}
