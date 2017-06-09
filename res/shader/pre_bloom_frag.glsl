#version 410

#include "uniforms.glsl"

// Input
uniform float uBloomThreshold;
uniform sampler2D uColorSampler;

// Output
layout (location = 0) out vec3 bloomBuffer;

// Extract bloom values from the main colors
void main()
{
    vec3 color = texture(uColorSampler, gl_FragCoord.xy / uRes).rgb;
    if (any(greaterThan(color, vec3(uBloomThreshold))))
        bloomBuffer = max(vec3(0), color - vec3(uBloomThreshold));
    else
        bloomBuffer = vec3(0);
}
