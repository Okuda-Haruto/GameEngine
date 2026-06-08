#pragma once
#include "KeyFrame.h"
#include "AnimationData.h"
#include "AnimationInterpolation.h"
#include <vector>

Vector3 LerpKeyFrame(std::vector<Keyframe_Vector3> keyFrame, AnimationInterpolation interpolation, float time);

Quaternion LerpKeyFrame(std::vector<Keyframe_Quaternion> keyFrame, AnimationInterpolation interpolation, float time);