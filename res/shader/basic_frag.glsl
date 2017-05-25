#version 410

// Lights (shading.glsl uses NUM_LIGHTS)
const int NUM_LIGHTS = 1;
vec3      LIGHT_POS[NUM_LIGHTS] = vec3[](vec3(1, 1, -3));
vec3      LIGHT_INT[NUM_LIGHTS] = vec3[](vec3(1));

#include "hg_sdf.glsl"
#include "uniforms.glsl"
#include "shading.glsl"

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


// Material
const int NUM_MATERIALS = 1;
vec3      ALBEDO[NUM_MATERIALS] = vec3[](vec3(1, 0, 0));
float     ROUGHNESS[NUM_MATERIALS] = float[](0.3);
float     METAL[NUM_MATERIALS] = float[](0);

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

vec3 getN(vec3 p)
{
    vec3 e = vec3(EPSILON, 0, 0);
    vec3 n = vec3(fScene(vec3(p + e.xyy)) - fScene(vec3(p - e.xyy)),
                  fScene(vec3(p + e.yxy)) - fScene(vec3(p - e.yxy)),
                  fScene(vec3(p + e.yyx)) - fScene(vec3(p - e.yyx)));
    return normalize(n);
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

    // Calculate ray to hit
    vec3 p = CAM_POS + depth * rd;

    // Directions to lights from hit
    vec3 lVecs[NUM_LIGHTS];
    for (int i = 0; i < NUM_LIGHTS; ++i) lVecs[i] = normalize(LIGHT_POS[i] - p);

    // Evaluate final shading
    fragColor = vec4(evalLighting(-rd, getN(p), lVecs, LIGHT_INT,
                                  ALBEDO[0], ROUGHNESS[0], METAL[0]), 1);
}
