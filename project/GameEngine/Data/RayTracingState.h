#pragma once
#include <stdint.h>
#include <Matrix4x4.h>
#include <Vector3.h>

struct RayTracingState
{
    int32_t windowWidth;
    int32_t windowHeight;
    float padding0[2];
    Matrix4x4 inverseViewMatrix;
    Matrix4x4 inverseProjectionMatrix;
    Vector3 cameraPosition;
    float padding1;
};
