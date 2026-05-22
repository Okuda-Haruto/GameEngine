#include "CopyImage.hlsli"

struct BoxFilterData
{
    int scale;
};

ConstantBuffer<BoxFilterData> gBoxFilterData : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput BoxFilter(float2 texcoord, int scale)
{
    uint width, height; // uvStepSize
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(float(width)), rcp(float(height)));
    
    PixelShaderOutput output;
    output.color.rgb = float3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
    
    const float averageKernel = 1.0f / (scale * scale);
    float2 index;
    for (int x = 0; x < scale; ++x)
    {
        index.x = float(x - scale / 2);
        for (int y = 0; y < scale; ++y)
        {
            index.y = float(y - scale / 2);
            
            float2 filterTexcoord = texcoord + index * uvStepSize;
            
            float3 fetchColor = gTexture.Sample(gSampler, filterTexcoord).rgb;
            output.color.rgb += fetchColor * averageKernel;
        }

    }
    
    return output;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    
    return BoxFilter(input.texcoord, gBoxFilterData.scale);
}