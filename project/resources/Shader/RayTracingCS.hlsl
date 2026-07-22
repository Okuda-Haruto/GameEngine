struct Ray
{
    float3 origin;
    float3 diff;
};

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

    // レイ方向 × edge2
    float3 pvec = cross(ray.diff, v20);

    // 行列式
    float det = dot(v10, pvec);

    // 平行判定
    if (abs(det) < 1e-6)
        return false;

    float invDet = 1.0 / det;

    // v0 → レイ始点
    float3 tvec = ray.origin - triangleVertex.v[0];

    // 重心座標 u
    float u = dot(tvec, pvec) * invDet;

    if (u < 0.0 || u > 1.0)
        return false;

    // tvec × edge1
    float3 qvec = cross(tvec, v10);

    // 重心座標 v
    float v = dot(ray.diff, qvec) * invDet;

    if (v < 0.0 || (u + v) > 1.0)
        return false;

    // レイ上の距離
    t = dot(edge2, qvec) * invDet;

    if (t < 1e-6)
        return false;

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
}