#include "CopyImage.hlsli"

struct VignetteData
{
    float vignetteIntensity;
    float vignetteCurve;
};

ConstantBuffer<VignetteData> gVignetteData : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    float2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    
    float vignette = correct.x * correct.y * gVignetteData.vignetteIntensity;
    
    vignette = saturate(pow(vignette, gVignetteData.vignetteCurve));
    
    output.color.rgb *= vignette;
    
    return output;
}