#version 410

uniform float uGT;

out vec4 fragColor;

void main()
{
    fragColor = vec4(0.5 + 0.5 * cos(uGT), 0.5 + 0.5 * sin(uGT), 1, 1);
}
