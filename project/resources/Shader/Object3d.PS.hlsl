#include "Object3d.hlsli"

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

struct Camera
{
    float3 WorldPosition;
};

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput PhangReflectionModel(VertexShaderOutput input, float4 textureColor, float cos)
{
    PixelShaderOutput output;
    float3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
    if (gMaterial.shininess > 0)
    {
        float3 toEye = normalize(gCamera.WorldPosition - input.worldPosition);
        float3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
        float RdotE = dot(reflectLight, toEye);
            
        float specularPow = pow(saturate(RdotE), gMaterial.shininess);
            
        float3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
        
        output.color.rgb = diffuse + specular;
    }
    else
    {
        output.color.rgb = diffuse;
    }
    output.color.a = gMaterial.color.a * textureColor.a;
    return output;
}

PixelShaderOutput BlinnPhangReflectionModel(VertexShaderOutput input, float4 textureColor, float cos)
{
    PixelShaderOutput output;
    float3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
    if (gMaterial.shininess > 0)
    {
        float3 toEye = normalize(gCamera.WorldPosition - input.worldPosition);
        float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
        float NdotH = dot(normalize(input.normal), halfVector);
            
        float specularPow = pow(saturate(NdotH), gMaterial.shininess);
            
        float3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
        
        output.color.rgb = diffuse + specular;
    }
    else
    {
        output.color.rgb = diffuse;
    }
    output.color.a = gMaterial.color.a * textureColor.a;
    return output;
}

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
        
        output = BlinnPhangReflectionModel(input,textureColor, cos);

    }
    else if (gMaterial.enableLighting == 1)     //Lambert
    {
        float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        
        output = BlinnPhangReflectionModel(input,textureColor, cos);
    }
    else    //none
    {
        output.color = gMaterial.color * textureColor;
    }
    return output;
}