#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct ColorChange
{
    int colorMode;
    float intensity;
};

ConstantBuffer<ColorChange> gColorChange : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput ChangeGrayScale(float4 color, float intensity)
{
    PixelShaderOutput output;
    
    float gray = dot(color.rgb, float3(0.2125f, 0.7154f, 0.0721f));
    
    float3 finalColor = lerp(color.rgb, gray, intensity);
    
    output.color = float4(gray.xxx, color.a);

    return output;
}

PixelShaderOutput ChangeSepiaTone(float4 color, float intensity)
{
    PixelShaderOutput output;
    
    float3 sepia;

    sepia.r = dot(color.rgb, float3(0.393, 0.769, 0.189));
    sepia.g = dot(color.rgb, float3(0.349, 0.686, 0.168));
    sepia.b = dot(color.rgb, float3(0.272, 0.534, 0.131));

    float3 finalColor = lerp(color.rgb, sepia, intensity);

    output.color = float4(finalColor, color.a);

    return output;
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    float4 color = gTexture.Sample(gSampler, input.texcoord);
  
    if (gColorChange.colorMode == 0)
    {
        output = ChangeGrayScale(color, gColorChange.intensity);
    }
    else if (gColorChange.colorMode == 1)
    {
        output = ChangeSepiaTone(color, gColorChange.intensity);
    }

    return output;
}