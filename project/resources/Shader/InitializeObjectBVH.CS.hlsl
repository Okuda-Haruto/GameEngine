struct Vertex
{
    float4 position;
    float2 texcoord;
    float3 normal;
};

//入力
StructuredBuffer<Vertex> gVertices : register(t0);
StructuredBuffer<uint> gIndices : register(t1);

struct ModelOffsetCount
{
    int vertexCount;
    int indexCount;
};
ConstantBuffer<ModelOffsetCount> gModelOffsetCount : register(b0);

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
    uint nodeCount;
};

//全体モデルBVHの要素数
RWStructuredBuffer<BVHData> gBVHData : register(u3);



struct Input
{
    uint rootNodeIndex;
};

RWStructuredBuffer<Input> gInput : register(u4);

RWStructuredBuffer<uint3> gKeepTriangles : register(uX);

//次の階層で作成するノード
RWStructuredBuffer<uint> gBuildNodeQueue : register(u5);

//現在の階層のノード数
RWStructuredBuffer<uint> gCurrentNodeCount : register(u6);

//次の階層のノード数
RWStructuredBuffer<uint> gNextNodeCount : register(u7);

static const int kMinBVHTriangleCount = 10;

void CreateChildNode(uint triangleStart, uint triangleCount, out int nodeIndex)
{
    gBoundingVolumes[nodeIndex].leftNodeIndex = -1;
    gBoundingVolumes[nodeIndex].rightNodeIndex = -1;
    
    uint index;
    InterlockedAdd(gBVHData[0].nodeCount, 1, index);
    
    nodeIndex = int(index);
    
    gBoundingVolumes[nodeIndex].triangleStart = triangleStart;
    gBoundingVolumes[nodeIndex].triangleCount = triangleCount;
        
    uint3 tri = gKeepTriangles[triangleStart];

    float3 v0 = gVertices[tri.x].position.xyz;
    float3 v1 = gVertices[tri.y].position.xyz;
    float3 v2 = gVertices[tri.z].position.xyz;

    AABB aabb;
    aabb.min = min(v0, min(v1, v2));
    aabb.max = max(v0, max(v1, v2));

    for (uint i = 1; i < triangleCount; ++i)
    {
        tri = gKeepTriangles[triangleStart + i];

        v0 = gVertices[tri.x].position.xyz;
        v1 = gVertices[tri.y].position.xyz;
        v2 = gVertices[tri.z].position.xyz;

        aabb.min.xyz = min(aabb.min.xyz, min(v0, min(v1, v2)));
        aabb.max.xyz = max(aabb.max.xyz, max(v0, max(v1, v2)));
    }
        
    gBoundingVolumes[nodeIndex].aabb = aabb;
        
    if (gBoundingVolumes[nodeIndex].triangleCount > kMinBVHTriangleCount)
    {
        uint queueIndex;
        InterlockedAdd(gNextNodeCount[0], 1, queueIndex);
        gBuildNodeQueue[queueIndex] = nodeIndex;
    }
}

void NewBoundingVolume(AABB baseAABB, int triangleStart, int triangleCount, out int leftNodeIndex, out int rightNodeIndex)
{
    leftNodeIndex = -1;
    rightNodeIndex = -1;
    
    if (triangleCount <= kMinBVHTriangleCount)
        return;
    
    float3 length = baseAABB.max - baseAABB.min;
    
    uint axis = 0;
    if (length.y >= length.x && length.y >= length.z)
    {
        axis = 1;
    }
    else if (length.z >= length.x && length.z >= length.y)
    {
        axis = 2;
    }
    
    float splitPosition = (baseAABB.min[axis] + baseAABB.max[axis]) / 2.0f;
    
    uint leftCount = 0;
    uint rightCount = 0;
    
    for (uint i = 0; i < triangleCount; ++i)
    {
        uint3 tri = gTriangles[triangleStart + i];
        float3 center = (
            gVertices[tri.x].position.xyz +
            gVertices[tri.y].position.xyz +
            gVertices[tri.z].position.xyz) / 3.0f;

        if (center[axis] < splitPosition)
            leftCount++;
        else
            rightCount++;
    }
    
    uint leftStart = triangleStart;
    uint rightStart = triangleStart + leftCount;
        
    uint leftOffset = 0;
    uint rightOffset = 0;
    
    for (uint i = 0; i < triangleCount; ++i)
    {
        uint3 tri = gTriangles[triangleStart + i];

        float3 center = (
                gVertices[tri.x].position.xyz +
                gVertices[tri.y].position.xyz +
                gVertices[tri.z].position.xyz) / 3.0f;

        if (center[axis] < splitPosition)
        {
            gKeepTriangles[leftStart + leftOffset] = tri;
            leftOffset++;
        }
        else
        {
            gKeepTriangles[rightStart + rightOffset] = tri;
            rightOffset++;
        }
    }
        
    if (leftOffset == 0 || rightOffset == 0)
        return;
        
    for (uint i = triangleStart; i < triangleStart + triangleCount; ++i)
    {
        gTriangles[i] = gKeepTriangles[i];
    }
        
    CreateChildNode(leftStart, leftOffset, leftNodeIndex);
    CreateChildNode(rightStart, rightOffset, rightNodeIndex);
    
}

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    gInput[0].rootNodeIndex = gBVHData[0].nodeCount;
    
    ++gBVHData[0].nodeCount;
    
    if (gModelOffsetCount.vertexCount == 0)
    {
        return;
    }
    
    AABB aabb;
    aabb.min = float4(gVertices[0].position);
    aabb.max = float4(gVertices[0].position);
    
    for (int i = 1; i < gModelOffsetCount.vertexCount; ++i)
    {
        aabb.min = min(aabb.min, gVertices[i].position);
        aabb.max = max(aabb.max, gVertices[i].position);
    }
    
    gBoundingVolumes[gInput[0].rootNodeIndex].aabb = aabb;
    
    gBoundingVolumes[gInput[0].rootNodeIndex].triangleStart = gBVHData[0].triangleCount;
    gBoundingVolumes[gInput[0].rootNodeIndex].triangleCount = 0;
    
    for (int i = 0; i < gModelOffsetCount.indexCount; i += 3)
    {
        gTriangles[gBVHData[0].triangleCount].xyz = float3(gIndices[i], gIndices[i + 1], gIndices[i + 2]);
        ++gBVHData[0].triangleCount;
    }
    
    gBoundingVolumes[gInput[0].rootNodeIndex].triangleCount = gBoundingVolumes[gInput[0].rootNodeIndex].triangleStart - gBVHData[0].triangleCount;

    gBoundingVolumes[gInput[0].rootNodeIndex].leftNodeIndex = 
    
    NewBoundingVolume(
        gBoundingVolumes[gInput[0].rootNodeIndex].aabb,
        gBoundingVolumes[gInput[0].rootNodeIndex].triangleStart,
        gBoundingVolumes[gInput[0].rootNodeIndex].triangleCount,
        gBoundingVolumes[gInput[0].rootNodeIndex].leftNodeIndex,
        gBoundingVolumes[gInput[0].rootNodeIndex].rightNodeIndex
    );

}