struct Material {
    vec3 albedo;
    float roughness;
    float metalness;
    vec3 emissivity;
};

Material brushedAlu = Material(vec3(0.913, 0.921, 0.925), 0.65, 1, vec3(0));
Material steel =      Material(vec3(0.24), 0.55, 1, vec3(0));
Material rust =       Material(vec3(0.23, 0.07, 0.01), 0.8, 0.8, vec3(0));
Material redPaint =   Material(vec3(0.913, 0, 0), 0.1, 0, vec3(0));
Material sand =       Material(vec3(0.5, 0.45, 0.2), 1, 0, vec3(0));
Material redPlasma =  Material(vec3(0), 0, 0, vec3(2, 0, 0));

Material mixMaterials(Material m1, Material m2, float k)
{
    return Material(mix(m1.albedo, m2.albedo, k),
                    mix(m1.roughness, m2.roughness, k),
                    mix(m1.metalness, m2.metalness, k),
                    mix(m1.emissivity, m2.emissivity, k));
}
