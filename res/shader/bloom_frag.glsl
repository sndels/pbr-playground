// Based on https://learnopengl.com/#!Advanced-Lighting/Bloom
#version 410

#include "uniforms.glsl"

// Input
uniform bool uHorizontal;
uniform float uLOD;
uniform sampler2D uBloomSampler;

// Output
layout (location = 0) out vec3 bloomBuffer;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    float lod = uHorizontal ? uLOD : 0.f;
    vec2 texCoords = gl_FragCoord.xy / uRes;
    vec2 texOffset = 1.0 / textureSize(uBloomSampler, 0); // gets size of single texel
    vec3 result = textureLod(uBloomSampler, texCoords, lod).rgb * weight[0]; // current fragment's contribution
    if(uHorizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += textureLod(uBloomSampler, texCoords + vec2(texOffset.x * i, 0.0), lod).rgb * weight[i];
            result += textureLod(uBloomSampler, texCoords - vec2(texOffset.x * i, 0.0), lod).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += textureLod(uBloomSampler, texCoords + vec2(0.0, texOffset.y * i), lod).rgb * weight[i];
            result += textureLod(uBloomSampler, texCoords - vec2(0.0, texOffset.y * i), lod).rgb * weight[i];
        }
    }
    bloomBuffer = result;
}
