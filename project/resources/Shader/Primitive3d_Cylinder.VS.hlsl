#include "Primitive.hlsli"

struct TransformationMatrix
{
    float4x4 ViewProjection;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct CylinderState
{
    float topRadius;
    float bottomRadius;
    float height;
    float segments;
};

ConstantBuffer<CylinderState> gCylinderState : register(b1);

float4 main(uint vertexId : SV_VertexID)
{
    VertexShaderOutput output;
    
    const float PI = 3.14159265359f;

    uint sideVertexCount = gCylinderState.segments * 6;

    float4 position = float4(0, 0, 0, 1);
    float2 texcoord = float2(0, 0);
    float3 normal = float3(0, 1, 0);
    
    if (gCylinderState.segments < 3)
    {
        return output;
    }
    
    if (vertexId < sideVertexCount)
    {
        uint segmentCount = vertexId / 6;
        uint vertexNumber = vertexId % 6;

        float angle = float(segmentCount) / gCylinderState.segments * PI * 2;
        float nextAngle = float(segmentCount + 1) / gCylinderState.segments * PI * 2;

        float3 bottom0 = float3(
            sin(angle) * gCylinderState.bottomRadius,
            -gCylinderState.height / 2.0f,
            cos(angle) * gCylinderState.bottomRadius);

        float3 bottom1 = float3(
            sin(nextAngle) * gCylinderState.bottomRadius,
            -gCylinderState.height / 2.0f,
            cos(nextAngle) * gCylinderState.bottomRadius);

        float3 top0 = float3(
            sin(angle) * gCylinderState.topRadius,
            gCylinderState.height / 2.0f,
            cos(angle) * gCylinderState.topRadius);

        float3 top1 = float3(
            sin(nextAngle) * gCylinderState.topRadius,
            gCylinderState.height / 2.0f,
            cos(nextAngle) * gCylinderState.topRadius);
        
        float sideNormalY = (gCylinderState.bottomRadius - gCylinderState.topRadius);

        // 2つの三角形で四角形を作る
        if (vertexNumber == 0 || vertexNumber == 3)
        {
            position = float4(top0, 1);
            texcoord = float2((float) segmentCount / gCylinderState.segments, 0);
            normal.x = normalize(float3(
                    sin(angle),
                    sideNormalY,
                    cos(angle)
            ));

        }
        else if (vertexNumber == 1)
        {
            position = float4(top1, 1);
            texcoord = float2((float) (segmentCount + 1) / gCylinderState.segments, 0);
            normal.x = normalize(float3(
                    sin(nextAngle),
                    sideNormalY,
                    cos(nextAngle)
            ));
        }
        else if (vertexNumber == 2 || vertexNumber == 4)
        {
            position = float4(bottom1, 1);
            texcoord = float2((float) (segmentCount + 1) / gCylinderState.segments, 1);
            normal.x = normalize(float3(
                    sin(nextAngle),
                    sideNormalY,
                    cos(nextAngle)
            ));
        }
        else
        {
            position = float4(bottom0, 1);
            texcoord = float2((float) segmentCount / gCylinderState.segments, 1);
            normal.x = normalize(float3(
                    sin(angle),
                    sideNormalY,
                    cos(angle)
            ));
        }
    }

    float4x4 WVP = mul(gTransformationMatrix.World, gTransformationMatrix.ViewProjection);
        
    output.position = position;
    output.texcoord = texcoord;
    output.normal = normal;
    
    return output;
}