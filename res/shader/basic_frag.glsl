#version 410

#include "hg_sdf.glsl"

uniform float uGT;
uniform vec2  uRes;

out vec4 fragColor;

float CAM_FOV = 65;

vec3 getViewRay(vec2 fragCoord, vec2 resolution, float fov)
{
    vec2 xy = fragCoord - resolution * 0.5;
    float z = resolution.y / tan(radians(fov * 0.5));
    return normalize(vec3(xy, z));
}

void main()
{
    fragColor = vec4(getViewRay(gl_FragCoord.xy, uRes, CAM_FOV), uGT);
}
