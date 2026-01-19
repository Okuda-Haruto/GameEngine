#pragma once
#include "KeyFrame.h"
#include "AnimationData.h"
#include <vector>

Vector3 LerpKeyFrame(std::vector<KeyFrame> keyFrame, AnimationData animationData, float t);