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
        if (particleIndex == 0)
        {
            gFreeCounter[0] = 0;
        }

    }

}