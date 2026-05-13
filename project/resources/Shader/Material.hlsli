struct Material
{
    float4 color;
    int reflection;
    int enableDirectionalLighting;
    int enablePointLighting;
    int enableSpotLighting;
    int enableEnviromentMap;
    float4x4 uvTransform;
    float shininess;
    int shading;
    float enviromentCoefficient;
};