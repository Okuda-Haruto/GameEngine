#include "CopyImage.hlsli"
#include "Random.hlsli"

struct VignetteData
{
    float vignetteIntensity;
    float vignetteCurve;
    int isUseRandom;
    float seed;
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
    
    if (gVignetteData.isUseRandom)
    {
        float random = rand2dTo1d(input.texcoord + float2(gVignetteData.seed, gVignetteData.seed));
        output.color.r = lerp(random, output.color.r, vignette);
        output.color.g = lerp(random, output.color.g, vignette);
        output.color.b = lerp(random, output.color.b, vignette);
    }
    else
    {
        output.color.rgb *= vignette;
    }
    
    return output;
}