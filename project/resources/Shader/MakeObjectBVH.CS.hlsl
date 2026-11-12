struct Vertex
{
    float4 position;
    float2 texcoord;
    float3 normal;
};

//入力
StructuredBuffer<Vertex> gVertices : register(t0);
StructuredBuffer<uint> gIndices : register(t1);



//3角形
RWStructuredBuffer<uint3> gTriangles : register(u0);

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

//BVH
RWStructuredBuffer<BoundingVolume> gBoundingVolumes : register(u1);

//ルートノード番号
RWStructuredBuffer<uint> gRootNodeIndex : register(u2);

struct BVHData
{
    uint triangleCount;
    uint boundingVolumesCount;
    uint rootNodeCount;
};

//全体モデルBVHの要素数
RWStructuredBuffer<BVHData> gBVHData : register(u3);



struct Input
{
    uint rootNodeIndex;
};

RWStructuredBuffer<Input> gInput : register(u4);

//次の階層で作成するノード
RWStructuredBuffer<uint> gBuildNodeQueue : register(u5);

//現在の階層のノード数
RWStructuredBuffer<uint> gCurrentNodeCount : register(u6);

//次の階層のノード数
RWStructuredBuffer<uint> gNextNodeCount : register(u7);


void AddTriangles(out uint minTriangleIndex, out uint maxTriangleIndex)
{
    //ここでgInputObjectDataのallocationをworldMatrix分ずらしてからTriangleを構成
    
    gTriangles[gBVHData[0].triangleCount].x = 

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
    
    if (gCurrentNodeCount)

}