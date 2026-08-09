struct Ray
{
    float3 origin;
    float3 diff;
};

struct AABB
{
    float4 min;
    float4 max;
};

bool IsCollision(AABB aabb, Ray ray)
{

    float3 min =
    {
        (aabb.min.x - ray.origin.x) / ray.diff.x,
		(aabb.min.y - ray.origin.y) / ray.diff.y,
		(aabb.min.z - ray.origin.z) / ray.diff.z,
    };
    float3 max =
    {
        (aabb.max.x - ray.origin.x) / ray.diff.x,
		(aabb.max.y - ray.origin.y) / ray.diff.y,
		(aabb.max.z - ray.origin.z) / ray.diff.z,
    };

    float tNearX = min(min.x, max.x), tFarX = max(min.x, max.x);
    float tNearY = min(min.y, max.y), tFarY = max(min.y, max.y);
    float tNearZ = min(min.z, max.z), tFarZ = max(min.z, max.z);

    float tmin = max(max(tNearX, tNearY), tNearZ);
    float tmax = min(min(tFarX, tFarY), tFarZ);

    if (tmin <= tmax && tmax >= 0.0f)
    {
        return true;
    }
    return false;
}

struct TriangleVertex
{
    float3 v[3];
};

bool RayTriangleIntersect(
    Ray ray,
    TriangleVertex triangleVertex,
    out float t,
    out float2 bary)
{
    float3 v10 = triangleVertex.v[1] - triangleVertex.v[0];
    float3 v20 = triangleVertex.v[2] - triangleVertex.v[0];
    
    float3 p = cross(ray.diff, v20);
    
    float divisor = dot(v10, p);
    if (abs(divisor ) < 1e-6) return false;

    float invDivisor = 1.0 / divisor;
    
    float3 d = ray.origin - triangleVertex.v[0];
    
    float u = dot(d, p) * invDivisor;
    if (u < 0.0 || u > 1.0) return false;
    
    float3 q = cross(d, v10);
    
    float v = dot(ray.diff, q) * invDivisor;
    if (v < 0.0 || (u + v) > 1.0) return false;
    
    t = dot(v20, q) * invDivisor;

    if (t < 1e-6) return false;

    bary = float2(u, v);

    return true;
}

struct RayTracingState
{
    int windowWidth;
    int windowHeight;
    float4x4 inverseViewMatrix;
    float4x4 inverseProjectionMatrix;
    float3 cameraPosition;
};

ConstantBuffer<RayTracingState> gState : register(b0);

struct Vertex
{
    float4 position;
    float2 texcoord;
    float3 normal;
};

StructuredBuffer<Vertex> gVertices : register(t0);

struct OffsetAllocation
{
    int vertexStart;
    int vertexCount;
    int indexStart;
    int indexCount;
};

struct OutputObjectData
{
    AABB rayTracingAABB;
    OffsetAllocation allocation;
};
StructuredBuffer<OutputObjectData> gObjectData : register(t1);


RWStructuredBuffer<int> gObjectDataCounter : register(u1);

RWTexture2D<float4> gTexture;

[numthreads(16, 16, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if (DTid.x >= gState.windowWidth ||
        DTid.y >= gState.windowHeight)
    {
        return;
    }
    
    float2 uv;
    uv.x = (DTid.x + 0.5f) / gState.windowWidth;
    uv.y = (DTid.y + 0.5f) / gState.windowHeight;
    float2 ndc;
    ndc.x = uv.x * 2.0f - 1.0f;
    ndc.y = 1.0f - uv.y * 2.0f;
            
    float4 clip = float4(ndc, 1.0f, 1.0f);
            
    float4 view = mul(clip, gState.inverseProjectionMatrix);
    view /= view.w;
            
    float4 world = mul(view, gState.inverseViewMatrix);
            
    Ray ray;
    ray.origin = gState.cameraPosition;
    ray.diff = normalize(world.xyz - gState.cameraPosition);
    for (int i = 0; i < gObjectDataCounter; i++)
    {
        if (IsCollision(gObjectData[i].rayTracingAABB, ray))
        {
            float t;
            float2 bary;
            
            for (int index = gObjectData[i].allocation.indexStart; index < gObjectData[i].allocation.indexCount; index += 3)
            {
                TriangleVertex vertexes;
                vertexes.v[0] = gVertices[gObjectData[i].allocation.vertexStart + index];
                vertexes.v[1] = gVertices[gObjectData[i].allocation.vertexStart + index + 1];
                vertexes.v[2] = gVertices[gObjectData[i].allocation.vertexStart + index + 2];
    
                if (RayTriangleIntersect(ray, vertexes, t, bary))
                {
                    
                }
            }
        }
    }
}