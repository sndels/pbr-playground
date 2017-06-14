// Camera uniforms
uniform float uCamPosX;
uniform float uCamPosY;
uniform float uCamPosZ;
uniform float uCamTargetX;
uniform float uCamTargetY;
uniform float uCamTargetZ;
uniform float uCamFov;

vec3 mouseLook(vec3 viewDir)
{
    float rotX = uMPos.x * PI ;
    float rotY = -uMPos.y * PI * 0.5; // TODO: Why is rotation around x the wrong way?
    return rotateY(rotateX(viewDir, rotY), rotX);
}

mat3 camOrient(vec3 eye, vec3 target, vec3 up)
{
    vec3 n = normalize(target - eye);
    vec3 u = normalize(cross(up, n));
    vec3 v = cross(n, u);
    return mat3(u, v, n);
}

vec3 getViewRay(vec2 fragCoord, vec2 resolution, float fov)
{
    vec2 xy = fragCoord - resolution * 0.5;
    float z = resolution.y / tan(radians(fov * 0.5));
    return normalize(vec3(xy, z));
}
