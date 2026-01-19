#include "InstanceObject3d.hlsli"

struct InstancingTransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
    float4 color;
};
StructuredBuffer<InstancingTransformationMatrix> gTransformationMatrixes : register(t0);

struct BoneMatrix
{
    float4x4 boneMatrix[128];
};
ConstantBuffer<BoneMatrix> gBoneMatrix : register(b1);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    uint4 boneIDs : BONE_ID0;
    float4 weights : WEIGHT0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    
    VertexShaderOutput output;
    
    if (input.weights.x > 0.0f ||
        input.weights.y > 0.0f ||
        input.weights.z > 0.0f ||
        input.weights.w > 0.0f)
    {
        float4 skinnedPos = float4(0, 0, 0, 0);
        float3 skinnedNormal = float3(0, 0, 0);
        [unroll]
        for (int i = 0; i < 4; ++i)
        {
            if (input.weights[i] >= 0)
            {
                float4 posTransformed = mul(input.position, gBoneMatrix.boneMatrix[input.boneIDs[i]]);
                skinnedPos += posTransformed * input.weights[i];

                float3 normalTransformed = mul(input.normal, (float3x3) gBoneMatrix.boneMatrix[input.boneIDs[i]]);
                skinnedNormal += normalTransformed * input.weights[i];
            }
        }
        
        output.position = mul(skinnedPos, gTransformationMatrixes[instanceId].WVP);
        output.normal = normalize(mul(skinnedNormal, (float3x3) gTransformationMatrixes[instanceId].WorldInverseTranspose));
        output.worldPosition = mul(skinnedPos, gTransformationMatrixes[instanceId].World).xyz;
    }
    else
    {
        output.position = mul(input.position, gTransformationMatrixes[instanceId].WVP);
        output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrixes[instanceId].WorldInverseTranspose));
        output.worldPosition = mul(input.position, gTransformationMatrixes[instanceId].World).xyz;
    }
    
    output.texcoord = input.texcoord;
    output.color = gTransformationMatrixes[instanceId].color;
    return output;
}