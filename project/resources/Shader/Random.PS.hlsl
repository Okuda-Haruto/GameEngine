#include "CopyImage.hlsli"
#include "Random.hlsli"

struct GPURandomSeed
{
    float seed;
};

ConstantBuffer<GPURandomSeed> gGPURandomSeed : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float random = rand2dTo1d(input.texcoord * gGPURandomSeed.seed);
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    output.color.r = lerp(textureColor.r ,random, 1.0f);
    output.color.g = lerp(textureColor.g, random, 1.0f);
    output.color.b = lerp(textureColor.b, random, 1.0f);
    
    return output;
}