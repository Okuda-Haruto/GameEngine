#pragma once
#include "Node.h"
#include "AnimationData.h"
#include "QuaternionTransform.h"
#include <memory>

QuaternionTransform GetAnimationTransform(std::weak_ptr<Node> node, AnimationData animationData, float time);