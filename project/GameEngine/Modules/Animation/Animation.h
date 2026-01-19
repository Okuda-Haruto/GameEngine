#pragma once
#include "Node.h"
#include "AnimationData.h"
#include "SRT.h"
#include <memory>

SRT GetAnimationTransform(std::weak_ptr<Node> node, AnimationData animationData, float time);