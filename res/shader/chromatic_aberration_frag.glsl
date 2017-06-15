#version 410

#include "uniforms.glsl"

// Inputs
uniform float uCAberrX;
uniform float uCAberrY;
uniform sampler2D uColorSampler;

// Output
layout (location = 0) out vec3 colorBuffer;

// Basic "chromatic aberration"
// TODO: Apply blur according to distance from center?
void main()
{
    vec2 texCoord = gl_FragCoord.xy / uRes;
    vec2 texOffset = (texCoord - 0.5) * vec2(uCAberrX, uCAberrY);
    vec3 color;
    color.r = texture(uColorSampler, clamp(texCoord - texOffset, 0.01, 0.99)).r;
    color.g = texture(uColorSampler, texCoord).g;
    color.b = texture(uColorSampler, clamp(texCoord + texOffset, 0.01, 0.99)).b;
    colorBuffer = color;
}
