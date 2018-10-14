#version 410

#include "uniforms.glsl"

// Inputs
uniform vec3 uBloom;
uniform sampler2D uSBloomSampler;
uniform sampler2D uMBloomSampler;
uniform sampler2D uLBloomSampler;
uniform sampler2D uColorSampler;

// Output
layout (location = 0) out vec3 colorBuffer;

// Combine bloom layers to main color
void main()
{
    vec2 texCoord = gl_FragCoord.xy / uRes;
    vec3 color = vec3(0);
    color += uBloom.x * texture(uSBloomSampler, texCoord).rgb;
    color += uBloom.y * texture(uMBloomSampler, texCoord).rgb;
    color += uBloom.z * texture(uLBloomSampler, texCoord).rgb;
    colorBuffer = texture(uColorSampler, texCoord).rgb + color;
}
