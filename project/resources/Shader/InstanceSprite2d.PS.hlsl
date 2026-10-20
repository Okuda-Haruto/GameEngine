#include "InstanceSprite2d.hlsli"
#include "Material.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 textureColor = gTexture.Sample(gSampler, input.UV);
    PixelShaderOutput output;
    output.color = input.color * textureColor;
    
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    return output;
}