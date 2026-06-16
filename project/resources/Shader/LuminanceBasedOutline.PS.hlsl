#include "CopyImage.hlsli"
#include "Material.hlsli"

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
Texture2D<float> gDepthTexture : register(t1);
SamplerState gSamplerPoint : register(s1);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

static const float kPrewittHrizonKernel[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};

static const float kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};

float Luminance(float3 v)
{
    return dot(v, float3(0.2125f, 0.7154f, 0.0721f));
}

PixelShaderOutput LuminanceBasedOutline(float2 texcoord)
{
    uint width, height; // uvStepSize
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(float(width)), rcp(float(height)));
    
    float2 difference = float2(0.0f, 0.0f);
    
    const float averageKernel = 1.0f / (3 * 3);
    float2 index;
    for (int x = 0; x < 3; ++x)
    {
        index.x = float(x - 3 / 2);
        for (int y = 0; y < 3; ++y)
        {
            index.y = float(y - 3 / 2);
            
            float2 filterTexcoord = texcoord + index * uvStepSize;
            float ndcDepth = gDepthTexture.Sample(gSamplerPoint, filterTexcoord);
            float4 viewSpace = mul(float4(0.0f, 0.0f, ndcDepth, 1.0f), gMaterial.projectionInverse);
            float viewZ = viewSpace.z * rcp(viewSpace.w);
            
            difference.x += viewZ * kPrewittHrizonKernel[x][y];
            difference.y += viewZ * kPrewittVerticalKernel[x][y];
        }

    }
    
    float weight = length(difference);
    weight = saturate(weight);
    
    PixelShaderOutput output;
    output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler,texcoord).rgb;
    output.color.a = 1.0f;
    
    return output;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    
    return LuminanceBasedOutline(input.texcoord);
}