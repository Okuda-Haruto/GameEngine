#pragma once
#include <Vector2.h>

struct RadialBlurData
{
    //中心点
    Vector2 center = {0.5f,0.5f};
    //ぼかしの幅
    float blurWidth = 0.01f;
};