#version 410

#include "uniforms.glsl"
#include "noise.glsl"

layout (location = 0) out float texOut;

void main() {
    texOut = fbm(vec3(gl_FragCoord.xy / uRes * 5, 1));
}
