vec3 lambertBRFD(vec3 albedo)
{
    return albedo / PI;
}

float ggx(float NoH, float rough)
{
    float a2 = rough * rough;
    a2 *= a2;
    float denom = NoH * NoH * (a2 - 1) + 1;
    return a2 / (PI * denom * denom);
}

vec3 schlick(float VoH, vec3 f0)
{
    return f0 + (1 - f0) * pow(1 - VoH, 5);
}

float schlick_ggx(float NoL, float NoV, float rough)
{
    float k = (rough + 1);
    k *= k * 0.125;
    float gl = NoL / (NoL * (1 - k) + k);
    float gv = NoV / (NoV * (1 - k) + k);
    return gl * gv;
}

vec3 cookTorranceBRDF(float NoL, float NoV, float NoH, float VoH, vec3 F, float rough)
{
    vec3 DFG = ggx(NoH, rough) * F * schlick_ggx(NoL, NoV, rough);
    float denom = 4 * NoL * NoV + 0.0001;
    return DFG / denom;
}

vec3 evalLighting(vec3 v, vec3 n, vec3 lVecs[NUM_LIGHTS], vec3 lInt[NUM_LIGHTS],
                  Material mat)
{
    vec3 sumCol = vec3(0);
    // Default fresnel
    vec3 f0 = mix(vec3(0.04), mat.albedo, mat.metalness);
    float NoV = max(dot(n, v), 0);
    for (int i = 0; i < NUM_LIGHTS; ++i) {
        // Dot products
        float NoL = max(dot(n, lVecs[i]), 0);
        vec3 h = normalize(v + lVecs[i]);
        float NoH = max(dot(n, h), 0);
        float VoH = max(dot(v, h), 0);

        // Fresnel
        vec3 F = schlick(VoH, f0);

        // Diffuse amount
        vec3 Ks = F;
        vec3 Kd = (1 - Ks) * (1 - mat.metalness);

        // Add light's contribution
        sumCol += (Kd * lambertBRFD(mat.albedo) +
                   cookTorranceBRDF(NoL, NoV, NoH, VoH, F, mat.roughness)) *
                  lInt[i] * NoL;
    }
    return sumCol;
}
