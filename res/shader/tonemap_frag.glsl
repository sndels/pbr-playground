#version 410

#include "uniforms.glsl"

// Inputs
uniform float uExposure;
uniform float uBloom;

// Hdr-input
uniform sampler2D uHdrSampler;
uniform sampler2D uBloomSampler;

// Output
out vec4 fragColor;

void main()
{
    const float gamma = 2.2;
    vec3 hdr = texture(uHdrSampler, gl_FragCoord.xy / uRes).rgb;
    // Apply bloom
    hdr += uBloom * texture(uBloomSampler, gl_FragCoord.xy / uRes).rgb;
    vec3 mapped = vec3(1) - exp(-hdr * uExposure);
    mapped = pow(mapped, vec3(1 / gamma));
    fragColor = vec4(mapped, 1);
}
