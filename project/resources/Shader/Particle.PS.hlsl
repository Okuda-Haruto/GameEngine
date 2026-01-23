#include "Particle.hlsli"
#include "Material.hlsli"

struct Fog
{
    float2 windowSize;
    float nearClip;
    float FarClip;
    float fogStart;
    float fogEnd;
    float4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<Fog> gFog : register(b1);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
Texture2D<float4> gDepthTexture : register(t1);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    PixelShaderOutput output;
    output.color = input.color * textureColor;
    
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    float2 screenUV = input.position.xy / gFog.windowSize;
    float depth = gDepthTexture.Sample(gSampler, screenUV.xy).r;
    
    if (input.position.z > depth)
    {
        discard;
    }
    
    float diff = depth - input.position.z;
    float fade = saturate(diff * 10000.0f);
    
    output.color.a = output.color.a * fade;
    
    return output;
}