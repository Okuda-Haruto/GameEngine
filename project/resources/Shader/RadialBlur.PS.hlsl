#include "CopyImage.hlsli"

struct RadialBlurData
{
    float2 center;
    float blurWidth;
};

ConstantBuffer<RadialBlurData> gRadialBlurData : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

static const int kNumSamples = 10;

PixelShaderOutput main(VertexShaderOutput input)
{
    float2 direction = input.texcoord - gRadialBlurData.center;
    float3 outputColor = float3(0.0f, 0.0f, 0.0f);
    
    for (int sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex)
    {
        float2 texcoord = input.texcoord + direction * gRadialBlurData.blurWidth * float(sampleIndex);
        outputColor.rgb += gTexture.Sample(gSampler, saturate(texcoord)).rgb;
    }
    outputColor.rgb *= rcp(float(kNumSamples));

    PixelShaderOutput output;
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
    return output;
}