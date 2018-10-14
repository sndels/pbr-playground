#version 410

#include "uniforms.glsl"
#include "hg_sdf.glsl"
#include "material.glsl"
#include "math.glsl"
#include "noise.glsl"

// Textures
uniform sampler2D uFbmSampler;

// Lights
const int NUM_LIGHTS = 1;
vec3      LIGHT_POS[NUM_LIGHTS] = vec3[](vec3(0, 3, 0));
vec3      LIGHT_INT[NUM_LIGHTS] = vec3[](vec3(20));

SceneResult scene(vec3 p)
{
    SceneResult sphere1 = SceneResult(fSphere(p - vec3(1, 0, 0), 0.75), 0);
    SceneResult sphere2 = SceneResult(fSphere(p - vec3(-1, 0, 0), 0.75), 1);
    SceneResult plane = SceneResult(fPlane(p, vec3(0, 1, 0), 2), 3);
    return opU(opU(sphere1, sphere2), plane);
}

Material evalMaterial(vec3 p, float matI)
{
    Material mat;
    if (matI < 1) {
        mat = mixMaterials(brushedAlu, redPlasma, clamp(pow(4 * fbm(p + 3.3), 8), 0, 1));
        if (length(mat.emissivity) > 0) mat.emissivity *= 0.5 * sin(uGT * 2) + 1.2;
    } else if (matI < 2) {
        mat = mixMaterials(steel, rust, clamp(pow(4 * fbm(p + 3.6), 8), 0, 1));
        if (mat.metalness < 0.9) mat.metalness += 0.5 * fbm(p * 8);
    } else {
        mat.albedo = vec3(0.005);
        mat.roughness = clamp(6 * pow(fbm((p + 3) * 0.4), 2), 0.0, 0.3);
        mat.metalness = 0;
        mat.emissivity = vec3(0);
    }
    return mat;
}

// Main sphere tracing -stuff
#include "sphere_tracing_end.glsl"
