#include "Primitive.hlsli"

static const uint kNumVertex = 36;
static const float4 kPositions[kNumVertex] =
{
    // +X
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, -1.0f, 1.0f },
    { 1.0f, -1.0f, -1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, -1.0f, -1.0f, 1.0f },
    { 1.0f, -1.0f, 1.0f, 1.0f },

    // -X
    { -1.0f, 1.0f, -1.0f, 1.0f },
    { -1.0f, 1.0f, 1.0f, 1.0f },
    { -1.0f, -1.0f, 1.0f, 1.0f },
    { -1.0f, 1.0f, -1.0f, 1.0f },
    { -1.0f, -1.0f, 1.0f, 1.0f },
    { -1.0f, -1.0f, -1.0f, 1.0f },

    // +Z
    { -1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, -1.0f, 1.0f, 1.0f },
    { -1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, -1.0f, 1.0f, 1.0f },
    { -1.0f, -1.0f, 1.0f, 1.0f },

    // -Z
    { 1.0f, 1.0f, -1.0f, 1.0f },
    { -1.0f, 1.0f, -1.0f, 1.0f },
    { -1.0f, -1.0f, -1.0f, 1.0f },
    { 1.0f, 1.0f, -1.0f, 1.0f },
    { -1.0f, -1.0f, -1.0f, 1.0f },
    { 1.0f, -1.0f, -1.0f, 1.0f },

    // +Y
    { -1.0f, 1.0f, -1.0f, 1.0f },
    { -1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { -1.0f, 1.0f, -1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, -1.0f, 1.0f },

    // -Y
    { -1.0f, -1.0f, 1.0f, 1.0f },
    { -1.0f, -1.0f, -1.0f, 1.0f },
    { 1.0f, -1.0f, -1.0f, 1.0f },
    { -1.0f, -1.0f, 1.0f, 1.0f },
    { 1.0f, -1.0f, -1.0f, 1.0f },
    { 1.0f, -1.0f, 1.0f, 1.0f },
};

static const float2 kTexcoords[kNumVertex] =
{
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 1.0f },
    
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 1.0f },
    
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 1.0f },
    
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 1.0f },
    
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 1.0f },
    
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 1.0f },
};

static const float3 kNormals[kNumVertex] =
{
    // +X
    { 1.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f },

    // -X
    { -1.0f, 0.0f, 0.0f },
    { -1.0f, 0.0f, 0.0f },
    { -1.0f, 0.0f, 0.0f },
    { -1.0f, 0.0f, 0.0f },
    { -1.0f, 0.0f, 0.0f },
    { -1.0f, 0.0f, 0.0f },

    // +Z
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 1.0f },

    // -Z
    { 0.0f, 0.0f, -1.0f },
    { 0.0f, 0.0f, -1.0f },
    { 0.0f, 0.0f, -1.0f },
    { 0.0f, 0.0f, -1.0f },
    { 0.0f, 0.0f, -1.0f },
    { 0.0f, 0.0f, -1.0f },

    // +Y
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },

    // -Y
    { 0.0f, -1.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f },
};

struct TransformationMatrix
{
    float4x4 ViewProjection;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

VertexShaderOutput main(uint vertexId : SV_VertexID)
{
    VertexShaderOutput output;
    
    float4x4 WVP = mul(gTransformationMatrix.World, gTransformationMatrix.ViewProjection);

    output.position = mul(kPositions[vertexId], WVP);
    output.texcoord = kTexcoords[vertexId];
    output.normal = normalize(mul(kNormals[vertexId], (float3x3) gTransformationMatrix.WorldInverseTranspose));

    return output;
}