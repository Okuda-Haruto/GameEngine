struct BoneMatrix
{
    float4x4 boneMatrix;
};
StructuredBuffer<BoneMatrix> gBoneMatrix : register(t0);

struct Vertex
{
    float4 position;
    float2 texcoord;
    float3 normal;
};
StructuredBuffer<Vertex> gInputVertices : register(t1);

struct VertexInfluence
{
    float4 weights;
    uint4 boneIDs;
};
StructuredBuffer<VertexInfluence> gInfluences : register(t2);

RWStructuredBuffer<Vertex> gOutputVertices : register(u0);

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
    int objectNumber;
};
ConstantBuffer<InputObjectData> gInputObjectData : register(b0);

[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint vertexIndex = DTid.x;
    if (vertexIndex < gInputObjectData.allocation.vertexCount)
    {
        Vertex input = gInputVertices[vertexIndex];
        VertexInfluence influence = gInfluences[vertexIndex];
        
        Vertex skinned;
        skinned.texcoord = input.texcoord;
        
        if (influence.weights.x > 0.0f ||
        influence.weights.y > 0.0f ||
        influence.weights.z > 0.0f ||
        influence.weights.w > 0.0f)
        {
            float totalWeight = influence.weights.x + influence.weights.y + influence.weights.z + influence.weights.w;
            float4 weight = influence.weights / totalWeight;
        
            skinned.position = float4(0, 0, 0, 0);
            skinned.normal = float3(0, 0, 0);
        [unroll]
            for (int i = 0; i < 4; ++i)
            {

                if (weight[i] > 0)
                {
                    float4 posTransformed = mul(input.position, gBoneMatrix[influence.boneIDs[i]].boneMatrix);
                    skinned.position += posTransformed * weight[i];

                    float3 normalTransformed = mul(input.normal, (float3x3) gBoneMatrix[influence.boneIDs[i]].boneMatrix);
                    skinned.normal += normalTransformed * weight[i];
                }
            }
        }
        else
        {
            skinned.position = input.position;
            skinned.normal = normalize(input.normal);
        }
        gOutputVertices[gInputObjectData.allocation.vertexStart + vertexIndex] = skinned;
    }
}