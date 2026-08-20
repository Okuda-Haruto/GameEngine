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
    const float FLT_MAX = 1e30f;
    
    float3 min;
    float3 max;
    
    if (abs(ray.diff.x) < 1e-4)
    {
        if (ray.origin.x < aabb.min.x ||
        ray.origin.x > aabb.max.x)
        {
            return false;
        }
        
        min.x = -FLT_MAX;
        max.x = FLT_MAX;
    }
    else
    {
        min.x = (aabb.min.x - ray.origin.x) / ray.diff.x;
        max.x = (aabb.max.x - ray.origin.x) / ray.diff.x;
    }
    
    if (abs(ray.diff.y) < 1e-4)
    {
        if (ray.origin.y < aabb.min.y ||
        ray.origin.y > aabb.max.y)
        {
            return false;
        }
        min.y = -FLT_MAX;
        max.y = FLT_MAX;
    }
    else
    {
        min.y = (aabb.min.y - ray.origin.y) / ray.diff.y;
        max.y = (aabb.max.y - ray.origin.y) / ray.diff.y;
    }
    
    if (abs(ray.diff.z) < 1e-4)
    {
        if (ray.origin.z < aabb.min.z ||
        ray.origin.z > aabb.max.z)
        {
            return false;
        }
        min.z = -FLT_MAX;
        max.z = FLT_MAX;
    }
    else
    {
        min.z = (aabb.min.z - ray.origin.z) / ray.diff.z;
        max.z = (aabb.max.z - ray.origin.z) / ray.diff.z;
    }

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

ConstantBuffer<RayTracingState> gRayTracingState : register(b0);

struct Vertex
{
    float4 position;
    float2 texcoord;
    float3 normal;
};

StructuredBuffer<Vertex> gVertices : register(t0);

StructuredBuffer<uint> gIndices : register(t1);

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
StructuredBuffer<OutputObjectData> gObjectData : register(t2);

StructuredBuffer<uint> gObjectDataCounter : register(t3);

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};
StructuredBuffer<DirectionalLight> gDirectionalLight : register(t4);

struct LightingState
{
    int numDirectionalLight;
};

ConstantBuffer<LightingState> gLightingState : register(b1);

RWTexture2D<float4> gTexture : register(u0);

[numthreads(16, 16, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if (DTid.x >= gRayTracingState.windowWidth ||
        DTid.y >= gRayTracingState.windowHeight)
    {
        return;
    }
    
    float2 uv;
    uv.x = (DTid.x + 0.5f) / gRayTracingState.windowWidth;
    uv.y = (DTid.y + 0.5f) / gRayTracingState.windowHeight;
    gTexture[DTid.xy] = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float2 ndc;
    ndc.x = uv.x * 2.0f - 1.0f;
    ndc.y = 1.0f - uv.y * 2.0f;
            
    float4 clip = float4(ndc, 1.0f, 1.0f);
            
    float4 view = mul(clip, gRayTracingState.inverseProjectionMatrix);
    view /= view.w;
            
    float4 world = mul(view, gRayTracingState.inverseViewMatrix);
            
    Ray ray;
    ray.origin = gRayTracingState.cameraPosition;
    ray.diff = normalize(world.xyz - gRayTracingState.cameraPosition);
    
    bool isHit = false;
    float minT = 0.0f;
    float3 hitNormal;
    float3 collisionPoint;
    
    for (int i = 0; i < gObjectDataCounter[0]; i++)
    {
        if (IsCollision(gObjectData[i].rayTracingAABB, ray))
        {
            
            float t;
            float2 bary;
            
            for (int index = gObjectData[i].allocation.indexStart; index < gObjectData[i].allocation.indexStart + gObjectData[i].allocation.indexCount; index += 3)
            {
                TriangleVertex vertexes;
                vertexes.v[0] = gVertices[gObjectData[i].allocation.vertexStart + gIndices[index]].position;
                vertexes.v[1] = gVertices[gObjectData[i].allocation.vertexStart + gIndices[index + 1]].position;
                vertexes.v[2] = gVertices[gObjectData[i].allocation.vertexStart + gIndices[index + 2]].position;
    
                if (RayTriangleIntersect(ray, vertexes, t, bary))
                {
                    if (t < minT || !isHit)
                    {
                        Vertex triangleVertex[3];
                        triangleVertex[0] = gVertices[gObjectData[i].allocation.vertexStart + gIndices[index]];
                        triangleVertex[1] = gVertices[gObjectData[i].allocation.vertexStart + gIndices[index + 1]];
                        triangleVertex[2] = gVertices[gObjectData[i].allocation.vertexStart + gIndices[index + 2]];
                    
                        hitNormal = normalize(triangleVertex[0].normal * (1.0 - bary.x - bary.y) +
                                       triangleVertex[1].normal * bary.x +
                                       triangleVertex[2].normal * bary.y);
                    
                        collisionPoint = ray.origin + ray.diff * t +
                                                  hitNormal * 1e-3;
                        isHit = true;
                        minT = t;

                    }
                }
                
            }
            
        }
    }
    
    if (isHit)
    {
        
        for (int lightIndex = 0; lightIndex < gLightingState.numDirectionalLight; lightIndex++)
        {
            float NdotL = dot(hitNormal, -gDirectionalLight[lightIndex].direction);

            if (NdotL <= 0)
            {
                continue;
            }
            
            Ray directionalShadowRay;
            directionalShadowRay.origin = collisionPoint;
            directionalShadowRay.diff = -gDirectionalLight[lightIndex].direction;

            for (int i = 0; i < gObjectDataCounter[0]; i++)
            {
                if (IsCollision(gObjectData[i].rayTracingAABB, directionalShadowRay))
                {
            
                    float t;
                    float2 bary;
            
                    for (int index = gObjectData[i].allocation.indexStart; index < gObjectData[i].allocation.indexStart + gObjectData[i].allocation.indexCount; index += 3)
                    {
                        TriangleVertex vertexes;
                        vertexes.v[0] = gVertices[gObjectData[i].allocation.vertexStart + gIndices[index]].position;
                        vertexes.v[1] = gVertices[gObjectData[i].allocation.vertexStart + gIndices[index + 1]].position;
                        vertexes.v[2] = gVertices[gObjectData[i].allocation.vertexStart + gIndices[index + 2]].position;
    
                        if (RayTriangleIntersect(directionalShadowRay, vertexes, t, bary))
                        {
                            gTexture[DTid.xy] = float4(0.0f, 0.0f, 0.0f, 0.5f);
                            return;
                        }
                
                    }
            
                }
            }
        }
    }

}