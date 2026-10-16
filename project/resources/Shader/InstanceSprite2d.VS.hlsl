#include "InstanceSprite2d.hlsli"

struct InstancingTransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
    float4 color;
};

StructuredBuffer<InstancingTransformationMatrix> gTransformationMatrix : register(t0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix[instanceId].WVP);
    output.texcoord = input.texcoord;
    output.color = gTransformationMatrix[instanceId].color;
    return output;
}