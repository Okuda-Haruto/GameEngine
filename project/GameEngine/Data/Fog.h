#pragma once
#include <Vector2.h>

struct Fog {
    Vector2 windowSize;
    float nearClip;
    float FarClip;
    float fogStart;
    float fogEnd;
    float padding;
};