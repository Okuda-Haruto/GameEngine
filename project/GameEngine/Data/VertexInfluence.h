#pragma once
#include "Vector4.h"
#include "UINT4.h"

struct VertexInfluence
{
    Vector4 weights;
    UINT4 boneIDs;
};