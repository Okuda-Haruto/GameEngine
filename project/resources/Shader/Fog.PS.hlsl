#include "Object3d.hlsli"

struct Fog
{
    float2 windowSize;
    float nearClip;
    float FarClip;
    float fogStart;
    float fogEnd;
    float4 color;
};

ConstantBuffer<Fog> gFog : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
Texture2D<float4> gDepthTexture : register(t1);
SamplerState gSampler : register(s0);

float Lerp(float f1, float f2, float t)
{
    return f1 * (1.0f - t) + f2 * t;
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gFog.color;
    
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    float2 screenUV = input.position.xy / gFog.windowSize;
    float depth = gDepthTexture.Sample(gSampler, screenUV.xy).r;

    float linerDepth = (2.0f * gFog.nearClip) / (gFog.FarClip + gFog.nearClip - input.position.z * (gFog.FarClip - gFog.nearClip));
    
    float fogFactor = saturate((linerDepth - gFog.fogStart) / (gFog.fogEnd - gFog.fogStart));
    
    output.color.a = Lerp(0.0f, 1.0f, fogFactor);
    
    return output;
}