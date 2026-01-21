#include "LerpKeyFrame.h"
#include "Lerp.h"
#include "Operation/Operation.h"

Vector3 LerpKeyFrame(std::vector<KeyFrame> keyFrame, AnimationData animationData, float t) {
	//60fpsからアニメーションに合わせる
	float time = t * float(animationData.FPS);

	//範囲内に合わせる
	time = std::min(float(animationData.duration), std::max(time, 0.0f));

	KeyFrame minKeyFrame = keyFrame[0];
	KeyFrame maxKeyFrame = keyFrame[keyFrame.size() - 1];
	for (const KeyFrame key : keyFrame) {
		//より近いフレームを選別
		if (key.time < time && 
			key.time > minKeyFrame.time) {
			minKeyFrame = key;
		} else if (key.time > time &&
			key.time < maxKeyFrame.time) {
			maxKeyFrame = key;
		}
	}

	Vector3 result;
	result = Lerp(minKeyFrame.vector, maxKeyFrame.vector, time / float(animationData.duration));

	return result;
}

Quaternion LerpKeyFrame(std::vector<QuaternionKeyFlame> keyFrame, AnimationData animationData, float t) {
	//60fpsからアニメーションに合わせる
	float time = t * float(animationData.FPS);

	//範囲内に合わせる
	time = std::min(float(animationData.duration), std::max(time, 0.0f));

	QuaternionKeyFlame minKeyFrame = keyFrame[0];
	QuaternionKeyFlame maxKeyFrame = keyFrame[keyFrame.size() - 1];
	for (const QuaternionKeyFlame key : keyFrame) {
		//より近いフレームを選別
		if (key.time < time &&
			key.time > minKeyFrame.time) {
			minKeyFrame = key;
		} else if (key.time > time &&
			key.time < maxKeyFrame.time) {
			maxKeyFrame = key;
		}
	}

	Quaternion result;
	result = Slerp(minKeyFrame.quaternion, maxKeyFrame.quaternion, time / float(animationData.duration));

	return result;
}