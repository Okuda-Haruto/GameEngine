#pragma once
#include "Node.h"
#include "AnimationData.h"
#include "AnimationInterpolation.h"
#include "QuaternionTransform.h"
#include <memory>
#include <vector>
#include <windows.h>

QuaternionTransform GetAnimationTransform(std::weak_ptr<Node> node, std::vector<AnimationData> animationData, UINT animationIndex, AnimationInterpolation interpolation, float time);