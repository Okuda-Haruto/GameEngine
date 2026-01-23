#include "Animation.h"
#include "LerpKeyFrame/LerpKeyFrame.h"
#include "Operation/Operation.h"

QuaternionTransform GetAnimationTransform(std::weak_ptr<Node> node, AnimationData animationData, AnimationInterpolation interpolation, float time) {
	QuaternionTransform result{};
	result.scale = { 1.0f,1.0f,1.0f };

	if (node.lock()->children.empty()) {
		if (!node.lock()->scaleKeyFrame.empty())result.scale = LerpKeyFrame(node.lock()->scaleKeyFrame, animationData, interpolation, time);
		if (!node.lock()->rotateKeyFrame.empty())result.rotate = LerpKeyFrame(node.lock()->rotateKeyFrame, animationData, interpolation, time);
		if (!node.lock()->translateKeyFrame.empty())result.translate = LerpKeyFrame(node.lock()->translateKeyFrame, animationData, interpolation, time);
		return result;
	} else {
		for (const std::shared_ptr<Node> child : node.lock()->children) {
			QuaternionTransform childTransform = GetAnimationTransform(child, animationData, interpolation, time);
			result.scale.x *= childTransform.scale.x;
			result.scale.y *= childTransform.scale.y;
			result.scale.z *= childTransform.scale.z;
			result.rotate = Multiply(result.rotate, childTransform.rotate);
			result.translate += childTransform.translate;
		}
		if (!node.lock()->scaleKeyFrame.empty()) {
			Vector3 scale = LerpKeyFrame(node.lock()->scaleKeyFrame, animationData, interpolation, time);
			result.scale.x *= scale.x;
			result.scale.y *= scale.y;
			result.scale.z *= scale.z;
		}
		if (!node.lock()->rotateKeyFrame.empty())result.rotate = Multiply(result.rotate, LerpKeyFrame(node.lock()->rotateKeyFrame, animationData, interpolation, time));
		if (!node.lock()->translateKeyFrame.empty())result.translate += LerpKeyFrame(node.lock()->translateKeyFrame, animationData, interpolation, time);
		return result;
	}
}