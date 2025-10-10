#include "Object3d.hlsli"

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

struct PointLight
{
    float4 color;
    float3 position;
    float intensity;
    float radius;
    float decay;
};

struct Camera
{
    float3 WorldPosition;
};

struct Material
{
    float4 color;
    int reflection;
    int enableDirectionalLighting;
    int enablePointLighting;
    float4x4 uvTransform;
    float shininess;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);

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
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        
        diffuseDirectionalLighting = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
    }
    
    float3 diffusePointLighting = { 0.0f, 0.0f, 0.0f };
    float3 pointLightDirection = { 0.0f, 0.0f, 0.0f };
    float factor = 0.0f;
    if (gMaterial.enablePointLighting)
    {  
        pointLightDirection = normalize(input.worldPosition - gPointLight.position);
        float distance = length(gPointLight.position - input.worldPosition);
        factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);
        
        float NdotL = dot(normalize(input.normal), -pointLightDirection);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        
        diffusePointLighting = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity * factor;
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
            specularDirectionalLighting = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
        }
        
        float3 specularPointLighting = { 0.0f, 0.0f, 0.0f };
        if (gMaterial.enablePointLighting)
        {   
            float3 reflectLight = reflect(pointLightDirection, normalize(input.normal));
            float RdotE = dot(reflectLight, toEye);
            
            float specularPow = pow(saturate(RdotE), gMaterial.shininess);
            specularPointLighting = gPointLight.color.rgb * gPointLight.intensity * factor * specularPow * float3(1.0f, 1.0f, 1.0f);
        }
        
        output.color.rgb = diffuseDirectionalLighting + specularDirectionalLighting + diffusePointLighting + specularPointLighting;
    }
    else
    {
        output.color.rgb = diffuseDirectionalLighting + diffusePointLighting;
    }
    output.color.a = gMaterial.color.a * textureColor.a;
    return output;
}



PixelShaderOutput BlinnPhangReflectionModel(VertexShaderOutput input, float4 textureColor)
{
    PixelShaderOutput output;
    float3 diffuseDirectionalLighting = { 0.0f, 0.0f, 0.0f };
    if (gMaterial.enableDirectionalLighting)
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        
        diffuseDirectionalLighting = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
    }
    
    float3 diffusePointLighting = { 0.0f, 0.0f, 0.0f };
    float3 pointLightDirection = { 0.0f, 0.0f, 0.0f };
    float factor = 0.0f;
    if (gMaterial.enablePointLighting)
    {
        pointLightDirection = normalize(input.worldPosition - gPointLight.position);
        float distance = length(gPointLight.position - input.worldPosition);
        factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);
        
        float NdotL = dot(normalize(input.normal), -pointLightDirection);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        
        diffusePointLighting = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity * factor;
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
        
        float3 specularPointLighting = { 0.0f, 0.0f, 0.0f };
        if (gMaterial.enablePointLighting)
        {
            float3 halfVector = normalize(-pointLightDirection + toEye);
            float NdotH = dot(normalize(input.normal), halfVector);
            
            float specularPow = pow(saturate(NdotH), gMaterial.shininess);
            
            specularPointLighting = gPointLight.color.rgb * gPointLight.intensity * factor * specularPow * float3(1.0f, 1.0f, 1.0f);
        }
        
        output.color.rgb = diffuseDirectionalLighting + specularDirectionalLighting + diffusePointLighting + specularPointLighting;
    }
    else
    {
        output.color.rgb = diffuseDirectionalLighting + diffusePointLighting;
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
        
    if (gMaterial.reflection == 2)          //HalfLambert
    {
        
        output = BlinnPhangReflectionModel(input,textureColor);

    }
    else if (gMaterial.reflection == 1)     //Lambert
    {
        float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        
        output = BlinnPhangReflectionModel(input,textureColor);
    }
    else    //none
    {
        output.color = gMaterial.color * textureColor;
    }
    return output;
}