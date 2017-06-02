#version 410

#include "uniforms.glsl"

// Inputs
uniform float uExposure;
uniform float uSBloom;
uniform float uMBloom;
uniform float uLBloom;

// Hdr-input
uniform sampler2D uHdrSampler;
uniform sampler2D uSBloomSampler;
uniform sampler2D uMBloomSampler;
uniform sampler2D uLBloomSampler;

// Output
out vec4 fragColor;

void main()
{
    const float gamma = 2.2;
    vec3 hdr = texture(uHdrSampler, gl_FragCoord.xy / uRes).rgb;
    // Apply bloom
    hdr += uSBloom * texture(uSBloomSampler, gl_FragCoord.xy / uRes).rgb;
    hdr += uMBloom * texture(uMBloomSampler, gl_FragCoord.xy / uRes).rgb;
    hdr += uLBloom * texture(uLBloomSampler, gl_FragCoord.xy / uRes).rgb;
    vec3 mapped = vec3(1) - exp(-hdr * uExposure);
    mapped = pow(mapped, vec3(1 / gamma));
    fragColor = vec4(mapped, 1);
}
