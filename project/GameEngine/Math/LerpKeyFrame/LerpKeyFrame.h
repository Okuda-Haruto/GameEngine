#pragma once
#include "KeyFrame.h"
#include "QuaternionKeyFlame.h"
#include "AnimationData.h"
#include <vector>

Vector3 LerpKeyFrame(std::vector<KeyFrame> keyFrame, AnimationData animationData, float t);

Quaternion LerpKeyFrame(std::vector<QuaternionKeyFlame> keyFrame, AnimationData animationData, float t);