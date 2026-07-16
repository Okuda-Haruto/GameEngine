#include "Random.hlsli"

struct EmitterSphere
{
    float3 translate;
    float radius;
    int count;
    float frequency;
    float frequencyTime;
    int emit;
};

ConstantBuffer<EmitterSphere> gEmitter : register(b0);

struct PerFrame
{
    float time;
    float deltaTime;
};

ConstantBuffer<PerFrame> gPerFrame : register(b1);

class RandomGenerator
{
    float3 seed;
    float3 Generate3d()
    {
        seed = rand3dTo3d(seed);
        return seed;
    }
    float Generate1d()
    {
        float result = rand3dTo1d(seed);
        seed.x = result;
        return result;
    }
};

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

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    
    if (gEmitter.emit != 0)
    {
        RandomGenerator generator;
        generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
        
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)        
        {
            int particleIndex;
            InterlockedAdd(gFreeCounter[0], 1, particleIndex);
            
            if (particleIndex < kMaxParticles)
            {
                gParticles[particleIndex].scale = generator.Generate3d();
                gParticles[particleIndex].translate = generator.Generate3d();
                gParticles[particleIndex].color.rgb = generator.Generate3d();
                gParticles[particleIndex].color.a = 1.0f;
            }

        }

    }
}