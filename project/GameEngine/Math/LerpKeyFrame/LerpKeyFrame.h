#pragma once
#include "KeyFrame.h"
#include "QuaternionKeyFlame.h"
#include "AnimationData.h"
#include "AnimationInterpolation.h"
#include <vector>

Vector3 LerpKeyFrame(std::vector<KeyFrame> keyFrame, AnimationData animationData, AnimationInterpolation interpolation, float t);

Quaternion LerpKeyFrame(std::vector<QuaternionKeyFlame> keyFrame, AnimationData animationData, AnimationInterpolation interpolation, float t);