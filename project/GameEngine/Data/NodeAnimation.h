#pragma once
#include <vector>
#include <Keyframe.h>

template <typename T>
struct AnimationCurve {
	std::vector<Keyframe<T>> keyframes;
};

struct NodeAnimation {
	AnimationCurve<Vector3> scale;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> translate;
};
