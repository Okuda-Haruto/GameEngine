#include "Particle.hlsli"

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
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 color : COLOR0;
};


VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrixes[instanceId].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrixes[instanceId].WorldInverseTranspose));
    output.worldPosition = mul(input.position, gTransformationMatrixes[instanceId].World).xyz;
    output.color = gTransformationMatrixes[instanceId].color;
    return output;
}