#pragma once
#include "Node.h"
#include "AnimationData.h"
#include "AnimationInterpolation.h"
#include "QuaternionTransform.h"
#include <memory>
#include <vector>
#include <windows.h>
#include <Joint.h>

void ApplyAnimation(Skeleton& skeleton, AnimationData& animationData, AnimationInterpolation interpolation, float time);

Skeleton CreateSkelton(std::shared_ptr<Node>& rootNode);