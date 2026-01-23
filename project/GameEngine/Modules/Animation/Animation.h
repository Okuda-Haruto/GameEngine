#pragma once
#include "Node.h"
#include "AnimationData.h"
#include "AnimationInterpolation.h"
#include "QuaternionTransform.h"
#include <memory>

QuaternionTransform GetAnimationTransform(std::weak_ptr<Node> node, AnimationData animationData, AnimationInterpolation interpolation, float time);