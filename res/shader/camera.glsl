// Camera
// TODO: Implement as rocket-tracks
vec3  CAM_POS = vec3(0, 5, -6);
vec3  CAM_TARGET = vec3(0, -1, 0);
vec3  CAM_UP = vec3(0, 1, 0);
float CAM_FOV = 45;

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
