#version 410

#include "uniforms.glsl"

// Input
uniform sampler2D uReflectionSampler;
uniform sampler2D uColorSampler;

// Output
layout (location = 0) out vec3 blurBuffer;

const float weights[25] = float[] (0.053836, 0.045591, 0.027687, 0.012055, 0.003763,
                                   0.045591, 0.038608, 0.023446, 0.010209, 0.003186,
                                   0.027687, 0.023446, 0.014238, 0.006200, 0.001935,
                                   0.012055, 0.010209, 0.006200, 0.002699, 0.000843,
                                   0.003763, 0.003186, 0.001935, 0.000843, 0.000263);

// Calculate blur with non-separated gaussian to exploit lod for different roughnesses
void main()
{
    vec2 texCoords = gl_FragCoord.xy / uRes;
    vec4 result = textureLod(uReflectionSampler, texCoords, 0).rgba;
    vec3 color = result.rgb;
    float roughness = result.a;
    if (roughness > 0) {
        if (roughness < 1) {
            float lod = 10 * roughness;
            vec3 centerColor = textureLod(uReflectionSampler, texCoords, lod).rgb * weights[0];
            color = -centerColor * 3; // compensate for extra weight from inner loop
            // Get texel size in mipmap
            vec2 texOffset = 1.0 / (textureSize(uColorSampler, 0) / pow(2, lod));
            // TODO: edge-aware with roughness and depth?
            // TODO: something less prone to aliasing and flicker than the mipmap-hack?
            for(int j = 0; j < 5; ++j) {
                for(int i = 0; i < 5; ++i) {
                    color += textureLod(uReflectionSampler, texCoords +
                                        vec2(texOffset.x * i, texOffset.y * j), lod).rgb *
                             weights[j * 5 + i];
                    color += textureLod(uReflectionSampler, texCoords +
                                        vec2(texOffset.x * i, -texOffset.y * j), lod).rgb *
                             weights[j * 5 + i];
                    color += textureLod(uReflectionSampler, texCoords +
                                        vec2(-texOffset.x * i, -texOffset.y * j), lod).rgb *
                             weights[j * 5 + i];
                    color += textureLod(uReflectionSampler, texCoords +
                                        vec2(-texOffset.x * i, texOffset.y * j), lod).rgb *
                             weights[j * 5 + i];
                }
            }
        } else {
            color = vec3(0);
        }
    }
    blurBuffer = color;
}
