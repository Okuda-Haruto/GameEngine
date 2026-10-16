#include "InstanceLine.hlsli"

struct InstancingTransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
    float4 color;
};
StructuredBuffer<InstancingTransformationMatrix> gTransformationMatrixes : register(t0);

struct VertexShaderInput
{
    float4 position : POSITION0;
};


VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrixes[instanceId].WVP);
    output.color = gTransformationMatrixes[instanceId].color;
    return output;
}