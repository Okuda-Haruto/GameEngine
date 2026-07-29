#include "CopyImage.hlsli"
struct DissolveData
{
    float threshold;
    float edgeWidth;
};

ConstantBuffer<DissolveData> gDissolveData : register(b0);
Texture2D<float4> gTexture : register(t0);
Texture2D<float4> gMaskTexture : register(t1);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float mask = gMaskTexture.Sample(gSampler, input.texcoord).r;
    if (mask <= gDissolveData.threshold)
    {
        PixelShaderOutput output;
        output.color = float4(0.0f, 0.0f, 0.0f, 1.0f);
        return output;
    }
    
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    float edge = 1.0f - smoothstep(gDissolveData.threshold, gDissolveData.threshold + gDissolveData.edgeWidth, mask);
    output.color.rgb += edge * float3(1.0f,0.4f,0.3f);

    return output;
}