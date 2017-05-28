#version 410

// These two should be included first
#include "uniforms.glsl"
#include "definitions.glsl"

// Lights (shading.glsl uses NUM_LIGHTS)
const int NUM_LIGHTS = 1;
vec3      LIGHT_POS[NUM_LIGHTS] = vec3[](vec3(5, 5, -8));
vec3      LIGHT_INT[NUM_LIGHTS] = vec3[](vec3(200));

#include "hg_sdf.glsl"
#include "shading.glsl"
#include "materials.glsl"
#include "noise.glsl"

out vec4 fragColor;

// Ray marching
const int   MAX_MARCHING_STEPS = 256;
const float MIN_DIST = 0;
const float MAX_DIST = 100;
const float EPSILON = 0.0001;

// Camera
vec3  CAM_POS = vec3(0, 1, -4);
vec3  CAM_TARGET = vec3(0, 0, 0);
vec3  CAM_UP = vec3(0, 1, 0);
float CAM_FOV = 65;

// Material
float ACTIVE_MATERIAL = 0;

// Textures
uniform sampler2D uFbmSampler;

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
    float sphereDist = fSphere(p - vec3(0), 0.75);
    float planeDist = fPlane(p, vec3(0, 1, 0), 2);
    ACTIVE_MATERIAL = sphereDist < planeDist ? 0 : 1;
    return min(sphereDist, planeDist);
}

// Don't call this before retrieving material from hit
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
        if (dist < depth * EPSILON) break;

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

    // Retrieve material for hit
    Material mat;
    if (ACTIVE_MATERIAL < 1) {
        mat = mixMaterials(steel, rust, clamp(pow(4 * fbm(p + 0.4), 8), 0, 1));
    } else {
        mat = sand;
        mat.metalness = 0.5 * clamp(fbm(p * 0.15 - vec3(0, sin(uGT * 0.2), uGT * 0.2)), 0, 1);
    }

    // Directions to lights from hit + intensities at hit
    vec3 lVecs[NUM_LIGHTS], lInts[NUM_LIGHTS];
    for (int i = 0; i < NUM_LIGHTS; ++i) {
        vec3 toLight = LIGHT_POS[i] - p;
        float lightDist = length(toLight);
        lVecs[i] = toLight / lightDist;
        lInts[i] = LIGHT_INT[i] / (lightDist * lightDist);
    }

    // Evaluate final shading
    fragColor = vec4(evalLighting(-rd, getN(p), lVecs, lInts, mat), 1);
}
