struct Vertex
{
    float4 position;
    float2 texcoord;
    float3 normal;
};

StructuredBuffer<Vertex> gVertices : register(t0);
struct AABB
{
    float4 min;
    float4 max;
};

struct OffsetAllocation
{
    int vertexStart;
    int vertexCount;
    int indexStart;
    int indexCount;
};

struct InputObjectData
{
    OffsetAllocation allocation;
    float4x4 worldMatrix;
};
ConstantBuffer<InputObjectData> gInputObjectData : register(b0);

struct OutputObjectData
{
    AABB rayTracingAABB;
    OffsetAllocation allocation;
};
RWStructuredBuffer<OutputObjectData> gOutPutObjectData : register(u0);

RWStructuredBuffer<uint> gObjectDataCounter : register(u1);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if (gInputObjectData.allocation.vertexCount == 0)
    {
        return;
    }
    
    AABB aabb;
    aabb.min = float4(gVertices[gInputObjectData.allocation.vertexStart].position);
    aabb.max = float4(gVertices[gInputObjectData.allocation.vertexStart].position);
    
    for (int i = 1; i < gInputObjectData.allocation.vertexCount; i++)
    {
        aabb.min = min(aabb.min, gVertices[gInputObjectData.allocation.vertexStart + i].position);
        aabb.max = max(aabb.max, gVertices[gInputObjectData.allocation.vertexStart + i].position);
    }

    gOutPutObjectData[gObjectDataCounter[0]].allocation = gInputObjectData.allocation;
    gOutPutObjectData[gObjectDataCounter[0]].rayTracingAABB = aabb;

    gObjectDataCounter[0]++;
}