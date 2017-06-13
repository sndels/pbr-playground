#include "camera.glsl"
#include "shading.glsl"

// Parameters
#define MAX_MARCHING_STEPS  256
#define MIN_DIST            0
#define MAX_DIST            100
#define EPSILON             0.0001

// Output
layout (location = 0) out vec4 hdrBuffer;
layout (location = 1) out vec4 hdrReflectionBuffer;
layout (location = 2) out vec3 normalBuffer;

struct HitInfo {
    vec3 color;
    vec3 normal;
    Material material;
};

vec3 getN(vec3 p)
{
    vec3 e = vec3(EPSILON, 0, 0);
    vec3 n = vec3(scene(vec3(p + e.xyy)).dist - scene(vec3(p - e.xyy)).dist,
                  scene(vec3(p + e.yxy)).dist - scene(vec3(p - e.yxy)).dist,
                  scene(vec3(p + e.yyx)).dist - scene(vec3(p - e.yyx)).dist);
    return normalize(n);
}

// TODO: Enhanced sphere tracing?
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

HitInfo evalHit(vec3 p, vec3 rd, float matI)
{
    HitInfo info;
    // Retrieve material for hit
    info.material = evalMaterial(p, matI);

    // Directions to lights from hit + intensities at hit
    vec3 lVecs[NUM_LIGHTS], lInts[NUM_LIGHTS];
    for (int i = 0; i < NUM_LIGHTS; ++i) {
        vec3 toLight = LIGHT_POS[i] - p;
        float lightDist = length(toLight);
        lVecs[i] = toLight / lightDist;
        lInts[i] = LIGHT_INT[i] / (lightDist * lightDist);
    }

    // Evaluate direct shading
    info.normal = getN(p);
    info.color = evalLighting(-rd, info.normal, lVecs, lInts, info.material) +
                 info.material.emissivity;
    return info;
}

void main()
{
    // Calculate view ray direction in scene space, include mouselook
    vec3 rayDir = getViewRay(gl_FragCoord.xy, uRes, CAM_FOV);
    rayDir = camOrient(CAM_POS, CAM_TARGET, CAM_UP) * rayDir;
    rayDir = mouseLook(rayDir);

    // Cast a ray into scene
    SceneResult result = castRay(rayDir, CAM_POS);

    // Check if it missed
    if (result.dist > MAX_DIST - EPSILON) return;

    // Calculate ray to hit
    vec3 viewRay = result.dist * rayDir;
    vec3 pos = CAM_POS + viewRay;

    // Evaluate hit
    HitInfo mainHit = evalHit(pos, rayDir, result.materialIndex);

    // Write primary ray to buffers
    hdrBuffer = vec4(mainHit.color, result.dist);
    normalBuffer = mainHit.normal;

    // Evaluate reflection
    // Early out if material is very rough
    if (mainHit.material.roughness > 0.99) {
        hdrReflectionBuffer = vec4(0, 0, 0, mainHit.material.roughness);
        return;
    }

    // Cast a ray into scene
    vec3 reflDir = reflect(rayDir, mainHit.normal);
    result = castRay(reflDir, pos);

    // Check if it missed, only write roughness to buffer
    if (result.dist > MAX_DIST - EPSILON) {
        hdrReflectionBuffer = vec4(0, 0, 0, mainHit.material.roughness);
        return;
    }

    // Calculate ray to hit
    vec3 reflRay = result.dist * reflDir;
    pos = pos + reflRay;

    // Evaluate hit
    HitInfo reflHit = evalHit(pos, reflDir, result.materialIndex);

    // Calculate intensity of reflection based on fresnel of primary surface
    vec3 f0 = mix(vec3(0.04), mainHit.material.albedo, mainHit.material.metalness);
    float VoH = max(dot(-rayDir, normalize(-rayDir + reflDir)), 0);
    vec3 F = schlick(VoH, f0);
    hdrReflectionBuffer = vec4(F * reflHit.color, mainHit.material.roughness);
}
