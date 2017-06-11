#version 410

#include "uniforms.glsl"

// Inputs
uniform sampler2D uColorSampler;
uniform sampler2D uBlurredReflectionSampler;

// Output
layout (location = 0) out vec3 colorBuffer;

// Mix blurred reflection with sharp reflection according to roughness and add to primary color
void main()
{
    vec2 texCoord = gl_FragCoord.xy / uRes;
    vec3 primaryColor = texture(uColorSampler, texCoord).rgb;
    vec3 blurredReflectionColor = texture(uBlurredReflectionSampler, texCoord).rgb;
    colorBuffer = primaryColor + blurredReflectionColor;
}
