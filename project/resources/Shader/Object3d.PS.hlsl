#include "Object3d.hlsli"

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    PixelShaderOutput output;
    output.color = gMaterial.color * textureColor;
    
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    if (gMaterial.enableLighting == 2)          //HalfLambert
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float4 lightColor;
        lightColor[0] = gDirectionalLight.color[0] * cos * gDirectionalLight.intensity;
        lightColor[1] = gDirectionalLight.color[1] * cos * gDirectionalLight.intensity;
        lightColor[2] = gDirectionalLight.color[2] * cos * gDirectionalLight.intensity;
        lightColor[3] = 1.0f;
        output.color = gMaterial.color * textureColor * lightColor;
    }
    else if (gMaterial.enableLighting == 1)     //Lambert
    {
        float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        float4 lightColor;
        lightColor[0] = gDirectionalLight.color[0] * cos * gDirectionalLight.intensity;
        lightColor[1] = gDirectionalLight.color[1] * cos * gDirectionalLight.intensity;
        lightColor[2] = gDirectionalLight.color[2] * cos * gDirectionalLight.intensity;
        lightColor[3] = 1.0f;
        output.color = gMaterial.color * textureColor * lightColor;
    }
    else    //none
    {
        output.color = gMaterial.color * textureColor;
    }
    return output;
}