#define INF 1.0 / 0

vec3 rotateX(vec3 p, float rad)
{
    float c = cos(rad);
    float s = sin(rad);
    return vec3(p.x, c * p.y - s * p.z, s * p.y + c * p.z);
}

vec3 rotateY(vec3 p, float rad)
{
    float c = cos(rad);
    float s = sin(rad);
    return vec3(c * p.x + s * p.z, p.y, -s * p.x + c * p.z);
}

vec3 rotateZ(vec3 p, float rad)
{
    float c = cos(rad);
    float s = sin(rad);
    return vec3(c * p.x - s * p.y, s * p.x + c * p.y, p.z);
}
