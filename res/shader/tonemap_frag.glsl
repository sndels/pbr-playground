#version 410

#include "uniforms.glsl"

// Inputs
uniform float uExposure;

// Hdr-input
uniform sampler2D uHdrSampler;

// Output
out vec4 fragColor;

// Apply exposure and gamma-correction
void main()
{
    const float gamma = 2.2;
    vec3 hdr = texture(uHdrSampler, gl_FragCoord.xy / uRes).rgb;
    vec3 mapped = vec3(1) - exp(-hdr * uExposure);
    mapped = pow(mapped, vec3(1 / gamma));
    fragColor = vec4(mapped, 1);
}
