#include "Animation.h"
#include "LerpKeyFrame/LerpKeyFrame.h"
#include "Operation/Operation.h"
#include "PrimitiveManager/PrimitiveManager.h"

QuaternionTransform GetAnimationTransform(std::weak_ptr<Node> node, std::vector<AnimationData> animationData, UINT animationIndex, AnimationInterpolation interpolation, float time) {
	QuaternionTransform result{};
	result.scale = { 1.0f,1.0f,1.0f };
	result.rotate = IdentityQuaternion();

	if (!node.lock()->scaleKeyFrame.empty())result.scale = LerpKeyFrame(node.lock()->scaleKeyFrame[animationIndex], animationData[animationIndex], interpolation, time);
	if (!node.lock()->rotateKeyFrame.empty())result.rotate = LerpKeyFrame(node.lock()->rotateKeyFrame[animationIndex], animationData[animationIndex], interpolation, time);
	if (!node.lock()->translateKeyFrame.empty())result.translate = LerpKeyFrame(node.lock()->translateKeyFrame[animationIndex], animationData[animationIndex], interpolation, time);
	
	float angle = 2.0f * acosf(result.rotate.w);
	Vector3 axis = Normalize(Vector3(result.rotate.x, result.rotate.y, result.rotate.z));

	return result;
}