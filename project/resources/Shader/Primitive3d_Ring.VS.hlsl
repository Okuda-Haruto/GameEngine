#include "Primitive.hlsli"

struct TransformationMatrix
{
    float4x4 ViewProjection;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct RingState
{
    float innerRadius;
    float outerRadius;
    float rate;
    float segments;
};

ConstantBuffer<RingState> gRingState : register(b1);

VertexShaderOutput main(uint vertexId : SV_VertexID)
{
    VertexShaderOutput output;
    
    const float PI = 3.14159265359f;

    float4 position = float4(0, 0, 0, 1);
    float2 texcoord = float2(0, 0);
    float3 normal = float3(0, 1, 0);
    
    // 1区画につき6頂点
    uint segmentCount = vertexId / 6;
    uint vertexNumber = vertexId % 6;

    float rate = saturate(gRingState.rate);

    uint drawSegments = uint(ceil(gRingState.segments * gRingState.rate));

    float angle = float(segmentCount) / gRingState.segments * 2.0f * PI;

    float nextAngle = min(float(segmentCount + 1), drawSegments) / gRingState.segments * 2.0f * PI;

    // 内側の円周上の頂点
    float3 inner0 = float3(
        sin(angle) * gRingState.innerRadius,
        0.0f,
        cos(angle) * gRingState.innerRadius
    );

    float3 inner1 = float3(
        sin(nextAngle) * gRingState.innerRadius,
        0.0f,
        cos(nextAngle) * gRingState.innerRadius
    );

    // 外側の円周上の頂点
    float3 outer0 = float3(
        sin(angle) * gRingState.outerRadius,
        0.0f,
        cos(angle) * gRingState.outerRadius
    );

    float3 outer1 = float3(
        sin(nextAngle) * gRingState.outerRadius,
        0.0f,
        cos(nextAngle) * gRingState.outerRadius
    );

    // 2つの三角形で四角形を作る
    if (vertexNumber == 0 || vertexNumber == 3)
    {
        position = float4(inner0, 1);
        texcoord = float2(float(segmentCount) / gRingState.segments, 0.0f);
    }
    else if (vertexNumber == 1)
    {
        position = float4(outer0, 1);
        texcoord = float2(float(segmentCount) / gRingState.segments, 1.0f);
    }
    else if (vertexNumber == 2 || vertexNumber == 4)
    {
        position = float4(outer1, 1);
        texcoord = float2(float(segmentCount + 1) / gRingState.segments, 1.0f);
    }
    else
    {
        position = float4(inner1, 1);
        texcoord = float2(float(segmentCount + 1) / gRingState.segments, 0.0f);
    }

    float4x4 WVP = mul(gTransformationMatrix.World, gTransformationMatrix.ViewProjection);

    output.position = mul(position, WVP);
    output.texcoord = texcoord;
    output.normal = normalize(
        mul(normal, (float3x3) gTransformationMatrix.WorldInverseTranspose)
    );

    return output;
}