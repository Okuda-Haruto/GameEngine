struct PerFrame
{
    float time;
    float deltaTime;
};

ConstantBuffer<PerFrame> gPerFrame : register(b0);

struct Particle
{
    float3 translate;
    float3 scale;
    float lifetime;
    float3 velocity;
    float currentTime;
    float4 color;
};

static const uint kMaxParticles = 1024;
RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeCounter : register(u1);

[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {
        if (gParticles[particleIndex].color.a != 0)
        {
            gParticles[particleIndex].translate += gParticles[particleIndex].velocity;
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            float alpha = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifetime);
            gParticles[particleIndex].color.a = saturate(alpha);
        }
    }
}