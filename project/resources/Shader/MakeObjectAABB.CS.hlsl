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

struct BoundingVolume
{
    AABB aabb;

    int leftNodeIndex;
    int rightNodeIndex;

    int triangleStart;
    int triangleCount;
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
//入力
ConstantBuffer<InputObjectData> gInputObjectData : register(b0);

//3角形
RWStructuredBuffer<uint> gTriangles : register(u0);
//現在の範囲の最大値を得る用
RWStructuredBuffer<uint> gTriangleCounter : register(u1);
//BVH
RWStructuredBuffer<BoundingVolume> gBoundingVolumes : register(u2);
//現在のBVH数
RWStructuredBuffer<uint> gBoundingVolumesCounter : register(u3);

struct OutputObjectData
{
    int rootBoundingVolumeIndex;
    float4x4 worldMatrix;
};
//出力。恐らくgBoundingVolumesの位置さえわかってればいい
RWStructuredBuffer<OutputObjectData> gOutPutObjectData : register(u4);
//出力数
RWStructuredBuffer<uint> gObjectDataCounter : register(u5);

void AddTriangles(out uint minTriangleIndex, out uint maxTriangleIndex)
{
    //ここでgInputObjectDataのallocationをworldMatrix分ずらしてからTriangleを構成
}

int NewBoundingVolume(AABB baseAABB, int triangleStart, int triangleCount)
{
    //BVH構成処理。ここでBVH内に三角形があるか、10個以下か等判定
}

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
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
    
    uint minTriangleIndex;
    uint maxTriangleIndex;
    AddTriangles(minTriangleIndex, maxTriangleIndex);
    
    gOutPutObjectData[gObjectDataCounter[0]].rootBoundingVolumeIndex = NewBoundingVolume(aabb, minTriangleIndex, maxTriangleIndex);

    gObjectDataCounter[0]++;
}