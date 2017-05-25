vec3 lambertBRFD(vec3 albedo)
{
    return albedo / PI;
}

vec3 evalLighting(vec3 v, vec3 n, vec3 albedo, vec3 lVecs[NUM_LIGHTS], vec3 lInts[NUM_LIGHTS])
{
    vec3 sumCol = vec3(0);
    for (int i = 0; i < NUM_LIGHTS; ++i) {
        // Add light's contribution
        sumCol += dot(lVecs[i], n) * lambertBRFD(albedo) * lInts[i];
    }
    return sumCol;
}
