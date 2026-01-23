#include "Object3d.hlsli"
#include "Material.hlsli"

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

struct SpotLight
{
    float4 color;
    float3 position;
    float intensity;
    float3 direction;
    float distance;
    float decay;
    float cosAngle;
    float cosFalloutStart;
};


struct Camera
{
    float3 WorldPosition;
    float nearDist;
    float farDist;
    float nearTransparentDist;
    float farTransparentDist;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);
ConstantBuffer<SpotLight> gSpotLight : register(b4);

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
    
    float3 diffusePointLighting = { 0.0f, 0.0f, 0.0f };
    float3 pointLightDirection = { 0.0f, 0.0f, 0.0f };
    float factor = 0.0f;
    if (gMaterial.enablePointLighting)
    {  
        pointLightDirection = normalize(input.worldPosition - gPointLight.position);
        float distance = length(gPointLight.position - input.worldPosition);
        factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);
        
        float cos = 1.0f;
        if (gMaterial.reflection == 2)
        {
            float NdotL = dot(normalize(input.normal), -pointLightDirection);
            cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        }
        else if (gMaterial.reflection == 1)
        {
            cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        }
        
        diffusePointLighting = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity * factor;
    }
    
    float3 diffuseSpotLighting = { 0.0f, 0.0f, 0.0f };
    float3 spotLightDirectionOnSurface = { 0.0f, 0.0f, 0.0f };
    float cosAngle = 0.0f;
    float falloutFactor = 0.0f;
    float attenuationFactor = 0.0f;
    if (gMaterial.enableSpotLighting)
    {
        spotLightDirectionOnSurface = normalize(gSpotLight.position - input.worldPosition);
        cosAngle = dot(spotLightDirectionOnSurface, normalize(gSpotLight.direction));
        falloutFactor = saturate((cosAngle - cos(gSpotLight.cosAngle)) / (cos(gSpotLight.cosFalloutStart) - cos(gSpotLight.cosAngle)));
        
        float distance = length(gSpotLight.position - input.worldPosition);
        attenuationFactor = pow(saturate(-distance / gSpotLight.distance + 1.0f), gSpotLight.decay);
        
        float cos = 1.0f;
        if (gMaterial.reflection == 2)
        {
            float NdotL = dot(normalize(input.normal), -spotLightDirectionOnSurface);
            cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        }
        else if (gMaterial.reflection == 1)
        {
            cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        }
        
        diffuseSpotLighting = gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * gSpotLight.intensity * falloutFactor;
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
        
        float3 specularPointLighting = { 0.0f, 0.0f, 0.0f };
        if (gMaterial.enablePointLighting)
        {   
            float3 reflectLight = reflect(pointLightDirection, normalize(input.normal));
            float RdotE = dot(reflectLight, toEye);
            
            float specularPow = pow(saturate(RdotE), gMaterial.shininess);
            specularPointLighting = gPointLight.color.rgb * gPointLight.intensity * factor * specularPow * float3(1.0f, 1.0f, 1.0f);
        }
        
        float3 specularSpotLighting = { 0.0f, 0.0f, 0.0f };
        if (gMaterial.enableSpotLighting)
        {
            float3 reflectLight = reflect(gSpotLight.direction, normalize(input.normal));
            float RdotE = dot(reflectLight, toEye);
            
            float specularPow = pow(saturate(RdotE), gMaterial.shininess);
            specularSpotLighting = gSpotLight.color.rgb * gSpotLight.intensity * falloutFactor * specularPow * float3(1.0f, 1.0f, 1.0f);
        }
        
        output.color.rgb = diffuseDirectionalLighting + specularDirectionalLighting + diffusePointLighting + specularPointLighting + diffuseSpotLighting + specularSpotLighting;
    }
    else
    {
        output.color.rgb = diffuseDirectionalLighting + diffusePointLighting + diffuseSpotLighting;
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
    
    float3 diffusePointLighting = { 0.0f, 0.0f, 0.0f };
    float3 pointLightDirection = { 0.0f, 0.0f, 0.0f };
    float factor = 0.0f;
    if (gMaterial.enablePointLighting)
    {
        pointLightDirection = normalize(input.worldPosition - gPointLight.position);
        float distance = length(gPointLight.position - input.worldPosition);
        factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);
        
        float cos = 1.0f;
        if (gMaterial.reflection == 2)
        {
            float NdotL = dot(normalize(input.normal), -pointLightDirection);
            cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        }
        else if (gMaterial.reflection == 1)
        {
            cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        }
        
        diffusePointLighting = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity * factor;
    }
    
    float3 diffuseSpotLighting = { 0.0f, 0.0f, 0.0f };
    float3 spotLightDirectionOnSurface = { 0.0f, 0.0f, 0.0f };
    float cosAngle = 0.0f;
    float falloutFactor = 0.0f;
    float attenuationFactor = 0.0f;
    if (gMaterial.enableSpotLighting)
    {
        spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight.position);
        cosAngle = dot(spotLightDirectionOnSurface, gSpotLight.direction);
        falloutFactor = saturate((cosAngle - cos(gSpotLight.cosAngle)) / (cos(gSpotLight.cosFalloutStart) - cos(gSpotLight.cosAngle)));
        
        float distance = length(gSpotLight.position - input.worldPosition);
        attenuationFactor = pow(saturate(-distance / gSpotLight.distance + 1.0f), gSpotLight.decay);
        
        float cos = 1.0f;
        if (gMaterial.reflection == 2)
        {
            float NdotL = dot(normalize(input.normal), -spotLightDirectionOnSurface);
            cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        }
        else if (gMaterial.reflection == 1)
        {
            cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        }
        
        diffuseSpotLighting = gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * cos * gSpotLight.intensity * attenuationFactor * falloutFactor;
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
       
        float3 specularSpotLighting = { 0.0f, 0.0f, 0.0f };
        if (gMaterial.enableSpotLighting)
        {
            float3 halfVector = normalize(-spotLightDirectionOnSurface + toEye);
            float NdotH = dot(normalize(input.normal), halfVector);
            
            float specularPow = pow(saturate(NdotH), gMaterial.shininess);
            specularSpotLighting = gSpotLight.color.rgb * gSpotLight.intensity * attenuationFactor * falloutFactor * specularPow * float3(1.0f, 1.0f, 1.0f);
        }
        
        output.color.rgb = diffuseDirectionalLighting + specularDirectionalLighting + diffusePointLighting + specularPointLighting + diffuseSpotLighting + specularSpotLighting;
    }
    else
    {
        output.color.rgb = diffuseDirectionalLighting + diffusePointLighting + diffuseSpotLighting;
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
    
    float3 diff = input.worldPosition - gCamera.WorldPosition;

    float dist = length(diff);
    
    float near = saturate((gCamera.nearDist - dist) / gCamera.nearTransparentDist);
    float far = saturate((dist - gCamera.farDist) / gCamera.farTransparentDist);

    output.color.a *= 1.0f - max(near, far);
    
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