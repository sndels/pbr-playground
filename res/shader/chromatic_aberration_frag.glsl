#version 410

#include "uniforms.glsl"

#define SAMPLES 32

// Inputs
uniform float uCAberr;
uniform sampler2D uColorSampler;

// Output
layout (location = 0) out vec3 colorBuffer;

// Basic "chromatic aberration"
void main()
{
    vec2 texCoord = gl_FragCoord.xy / uRes;
    vec2 dir = 0.5 - texCoord;
    vec2 caOffset = dir * uCAberr * 0.1;
    vec2 blurStep = caOffset * 0.06;

    vec3 sum = vec3(0);
    for (int i = 0; i < SAMPLES; i++) {
        sum += vec3(texture(uColorSampler, texCoord + caOffset + i * blurStep).r,
                    texture(uColorSampler, texCoord + i * blurStep).g,
                    texture(uColorSampler, texCoord - caOffset + i * blurStep).b);
    }

    colorBuffer = sum / SAMPLES;
}
