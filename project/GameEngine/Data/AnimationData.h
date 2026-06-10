#pragma once
#include <NodeAnimation.h>
#include <map>
#include <string>

struct AnimationData {
	float duration;	//再生時間(秒)
	//NodeAnimation
	std::map<std::string, NodeAnimation> nodeAnimations;
};