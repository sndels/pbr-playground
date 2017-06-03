#version 410

// These are possibly used by other includes
#include "uniforms.glsl"
#include "material.glsl"
#include "math.glsl"

// Lights (shading.glsl uses NUM_LIGHTS)
const int NUM_LIGHTS = 1;
vec3      LIGHT_POS[NUM_LIGHTS] = vec3[](vec3(1, 1, -2));
vec3      LIGHT_INT[NUM_LIGHTS] = vec3[](vec3(20));

#include "hg_sdf.glsl"
#include "shading.glsl"
#include "noise.glsl"

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

// Uniforms
uniform float uBloomThreshold;

// Textures
uniform sampler2D uFbmSampler;

// Output
layout (location = 0) out vec3 posBuffer;
layout (location = 1) out vec3 hdrBuffer;
layout (location = 2) out vec3 bloomBuffer;

mat3 camOrient(vec3 eye, vec3 target, vec3 up)
{
    vec3 n = normalize(target - eye);
    vec3 u = normalize(cross(up, n));
    vec3 v = cross(n, u);
    return mat3(u, v, n);
}

vec3 mouseLook(vec3 viewDir)
{
    float rotX = uMPos.x * PI ;
    float rotY = -uMPos.y * PI * 0.5; // TODO: Why is rotation around x the wrong way?
    return rotateY(rotateX(viewDir, rotY), rotX);
}

vec3 getViewRay(vec2 fragCoord, vec2 resolution, float fov)
{
    vec2 xy = fragCoord - resolution * 0.5;
    float z = resolution.y / tan(radians(fov * 0.5));
    return normalize(vec3(xy, z));
}

SceneResult scene(vec3 p)
{
    SceneResult sphere1 = SceneResult(fSphere(p - vec3(1, 0, 0), 0.75 + uPulse), 0);
    SceneResult sphere2 = SceneResult(fSphere(p - vec3(-1, 0, 0), 0.75 + uPulse), 1);
    SceneResult plane = SceneResult(fPlane(p, vec3(0, 1, 0), 2), 2);
    return opU(opU(sphere1, sphere2), plane);
}

vec3 getN(vec3 p)
{
    vec3 e = vec3(EPSILON, 0, 0);
    vec3 n = vec3(scene(vec3(p + e.xyy)).dist - scene(vec3(p - e.xyy)).dist,
                  scene(vec3(p + e.yxy)).dist - scene(vec3(p - e.yxy)).dist,
                  scene(vec3(p + e.yyx)).dist - scene(vec3(p - e.yyx)).dist);
    return normalize(n);
}

SceneResult castRay(vec3 rd, vec3 ro)
{
    float depth = MIN_DIST;
    SceneResult result;
    for (int i = 0; i < MAX_MARCHING_STEPS; ++i) {
        result = scene(ro + depth * rd);
        if (result.dist < depth * EPSILON) break;

        depth += result.dist;

        if (depth > MAX_DIST) break;
    }
    result.dist = depth;
    return result;
}

void main()
{
    // Calculate view ray direction in scene space, include mouselook
    vec3 rd = getViewRay(gl_FragCoord.xy, uRes, CAM_FOV);
    rd = camOrient(CAM_POS, CAM_TARGET, CAM_UP) * rd;
    rd = mouseLook(rd);

    // Cast a ray into scene
    SceneResult result = castRay(rd, CAM_POS);

    // Check if it missed
    if (result.dist > MAX_DIST - EPSILON) {
        posBuffer = vec3(0);
        hdrBuffer = vec3(0);
        bloomBuffer = vec3(0);
        return;
    }

    // Calculate ray to hit
    vec3 vr = result.dist * rd;
    vec3 p = CAM_POS + vr;

    // Retrieve material for hit
    Material mat;
    if (result.material < 1) {
        mat = mixMaterials(steel, rust, clamp(pow(4 * fbm(p + 3.6), 8), 0, 1));
        if (mat.metalness < 0.9) mat.metalness += 0.5 * fbm(p * 8);
    } else if (result.material < 2) {
        mat = mixMaterials(steel, redPlasma, clamp(pow(4 * fbm(p + 3.3), 8), 0, 1));
        if (length(mat.emissivity) > 0) mat.emissivity *= 0.5 * sin(uGT * 2) + 1.2;
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
    posBuffer = vr;
    vec3 color = evalLighting(-rd, getN(p), lVecs, lInts, mat) + mat.emissivity;
    hdrBuffer = color;
    // Write value to bloom-buffer if bright enough
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (any(greaterThan(color, vec3(uBloomThreshold))))
        bloomBuffer = max(vec3(0), color - vec3(uBloomThreshold));
    else bloomBuffer = vec3(0);
}
