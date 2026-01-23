struct Material
{
    float4 color;
    int reflection;
    int enableDirectionalLighting;
    int enablePointLighting;
    int enableSpotLighting;
    float4x4 uvTransform;
    float shininess;
    int shading;
};