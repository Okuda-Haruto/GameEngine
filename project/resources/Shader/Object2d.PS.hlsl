#include "Object3d.hlsli"
#include "Material.hlsli"

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

struct Camera
{
    float3 WorldPosition;
    float nearDist;
    float farDist;
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

PixelShaderOutput PhangReflectionModel(VertexShaderOutput input, float4 textureColor)
{
    PixelShaderOutput output;
    
    float3 diffuseDirectionalLighting = { 0.0f, 0.0f, 0.0f };
    if (gMaterial.enableDirectionalLighting)
    {
        float cos = 1.0f;
        if (gMaterial.reflection == 2)
        {
            float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
            cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        }
        else if (gMaterial.reflection == 1)
        {
            cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        }
        
        diffuseDirectionalLighting = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
    }
    
    if (gMaterial.shininess > 0)
    {
        float3 toEye = normalize(gCamera.WorldPosition - input.worldPosition);
            
        float3 specularDirectionalLighting = { 0.0f, 0.0f, 0.0f };
        if (gMaterial.enableDirectionalLighting)
        {
            float3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
            float RdotE = dot(reflectLight, toEye);
            
            float specularPow = pow(saturate(RdotE), gMaterial.shininess);
            specularDirectionalLighting = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
        }
        
        output.color.rgb = diffuseDirectionalLighting + specularDirectionalLighting;
    }
    else
    {
        output.color.rgb = diffuseDirectionalLighting;
    }
    return output;
}



PixelShaderOutput BlinnPhangReflectionModel(VertexShaderOutput input, float4 textureColor)
{
    PixelShaderOutput output;
    float3 diffuseDirectionalLighting = { 0.0f, 0.0f, 0.0f };
    if (gMaterial.enableDirectionalLighting)
    {
        float cos = 1.0f;
        if (gMaterial.reflection == 2)
        {
            float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
            cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        }
        else if (gMaterial.reflection == 1)
        {
            cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        }
        
        diffuseDirectionalLighting = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
    }
    
    if (gMaterial.shininess > 0)
    {
        float3 toEye = normalize(gCamera.WorldPosition - input.worldPosition);
        
        float3 specularDirectionalLighting = { 0.0f, 0.0f, 0.0f };
        if (gMaterial.enableDirectionalLighting)
        {
            float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
            float NdotH = dot(normalize(input.normal), halfVector);
            
            float specularPow = pow(saturate(NdotH), gMaterial.shininess);
            
            specularDirectionalLighting = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
        }
        
        output.color.rgb = diffuseDirectionalLighting + specularDirectionalLighting;
    }
    else
    {
        output.color.rgb = diffuseDirectionalLighting;
    }
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
    
    if (gMaterial.reflection <= 0 || gMaterial.reflection > 2)
        return output;
        
    if (gMaterial.shading == 0)
    {
        output = PhangReflectionModel(input, textureColor);
    }
    else
    {
        output = BlinnPhangReflectionModel(input, textureColor);
    }
    
    return output;
}