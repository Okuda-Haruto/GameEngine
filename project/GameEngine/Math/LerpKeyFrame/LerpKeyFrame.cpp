#include "LerpKeyFrame.h"
#include "Lerp.h"
#include "Operation/Operation.h"

Vector3 LerpKeyFrame(std::vector<KeyFrame> keyFrame, AnimationData animationData, AnimationInterpolation interpolation, float t) {
	//60fpsからアニメーションに合わせる
	float time = t * float(animationData.FPS);

	//範囲内に合わせる
	time = std::min(float(keyFrame[keyFrame.size() - 1].time), std::max(time, float(keyFrame[0].time)));

	KeyFrame minKeyFrame = keyFrame[0];
	uint32_t minIndex = 0;
	KeyFrame maxKeyFrame = keyFrame[keyFrame.size() - 1];
	uint32_t maxIndex = int(keyFrame.size()) - 1;
	for (uint32_t index = 0; index < keyFrame.size();index++) {
		//より近いフレームを選別
		if (keyFrame[index].time < time &&
			keyFrame[index].time > minKeyFrame.time) {
			minKeyFrame = keyFrame[index];
			minIndex = index;
		} else if (keyFrame[index].time > time &&
			keyFrame[index].time < maxKeyFrame.time) {
			maxKeyFrame = keyFrame[index];
			maxIndex = index;
		}
	}

	//同値だとダメ
	if (minKeyFrame.time == maxKeyFrame.time) {
		if (minKeyFrame.time <= keyFrame[0].time + 1.0f) {
			maxKeyFrame.time = keyFrame[1].time;
			maxIndex = 1;
		} else {
			minKeyFrame.time = keyFrame[keyFrame.size() - 2].time;
			minIndex = int(keyFrame.size()) - 2;
		}
	}

	Vector3 result;
	switch (interpolation)
	{
	case AnimationInterpolation::Linear:
		result = Lerp(minKeyFrame.vector, maxKeyFrame.vector, float(time - minKeyFrame.time) / float(maxKeyFrame.time - minKeyFrame.time));
		break;
	case AnimationInterpolation::Step:
		if (time - minKeyFrame.time <= maxKeyFrame.time - time) {
			result = minKeyFrame.vector;
		} else {
			result = maxKeyFrame.vector;
		}
		break;
	case AnimationInterpolation::Cubic_Spline:
		Vector3 vector[4];
		if (minIndex <= 0) {
			vector[0] = keyFrame[minIndex].vector;
			vector[1] = keyFrame[minIndex].vector;
			vector[2] = keyFrame[maxIndex].vector;
			vector[3] = keyFrame[maxIndex + 1].vector;
		} else if (maxIndex >= keyFrame.size() - 1) {
			vector[0] = keyFrame[minIndex - 1].vector;
			vector[1] = keyFrame[minIndex].vector;
			vector[2] = keyFrame[maxIndex].vector;
			vector[3] = keyFrame[maxIndex].vector;
		} else {
			vector[0] = keyFrame[minIndex - 1].vector;
			vector[1] = keyFrame[minIndex].vector;
			vector[2] = keyFrame[maxIndex].vector;
			vector[3] = keyFrame[maxIndex + 1].vector;
		}

		result = Spline(vector[0], vector[1], vector[2], vector[3], float(time - minKeyFrame.time) / float(maxKeyFrame.time - minKeyFrame.time));
		break;
	default:
		break;
	}

	return result;
}

Quaternion LerpKeyFrame(std::vector<QuaternionKeyFlame> keyFrame, AnimationData animationData, AnimationInterpolation interpolation, float t) {
	//60fpsからアニメーションに合わせる
	float time = t * float(animationData.FPS);

	//範囲内に合わせる
	time = std::min(float(animationData.duration), std::max(time, float(keyFrame[0].time)));

	QuaternionKeyFlame minKeyFrame = keyFrame[0];
	uint32_t minIndex = 0;
	QuaternionKeyFlame maxKeyFrame = keyFrame[keyFrame.size() - 1];
	uint32_t maxIndex = int(keyFrame.size()) - 1;
	for (uint32_t index = 0; index < keyFrame.size(); index++) {
		//より近いフレームを選別
		if (keyFrame[index].time < time &&
			keyFrame[index].time > minKeyFrame.time) {
			minKeyFrame = keyFrame[index];
			minIndex = index;
		} else if (keyFrame[index].time > time &&
			keyFrame[index].time < maxKeyFrame.time) {
			maxKeyFrame = keyFrame[index];
			maxIndex = index;
		}
	}

	//同値だとダメ
	if (minKeyFrame.time == maxKeyFrame.time) {
		if (minKeyFrame.time <= keyFrame[0].time + 1.0f) {
			maxKeyFrame.time = keyFrame[1].time;
			maxIndex = 1;
		} else {
			minKeyFrame.time = keyFrame[keyFrame.size() - 2].time;
			minIndex = int(keyFrame.size()) - 2;
		}
	}

	Quaternion result;
	switch (interpolation)
	{
	case AnimationInterpolation::Linear:
		result = Slerp(minKeyFrame.quaternion, maxKeyFrame.quaternion, float(time - minKeyFrame.time) / float(maxKeyFrame.time - minKeyFrame.time));
		break;
	case AnimationInterpolation::Step:
		if (time - minKeyFrame.time <= maxKeyFrame.time - time) {
			result = minKeyFrame.quaternion;
		} else {
			result = maxKeyFrame.quaternion;
		}
		break;
	case AnimationInterpolation::Cubic_Spline:
		Quaternion quaternion[4];
		if (minIndex <= 0) {
			quaternion[0] = keyFrame[minIndex].quaternion;
			quaternion[1] = keyFrame[minIndex].quaternion;
			quaternion[2] = keyFrame[maxIndex].quaternion;
			quaternion[3] = keyFrame[maxIndex + 1].quaternion;
		} else if (maxIndex >= keyFrame.size() - 1) {
			quaternion[0] = keyFrame[minIndex - 1].quaternion;
			quaternion[1] = keyFrame[minIndex].quaternion;
			quaternion[2] = keyFrame[maxIndex].quaternion;
			quaternion[3] = keyFrame[maxIndex].quaternion;
		} else {
			quaternion[0] = keyFrame[minIndex - 1].quaternion;
			quaternion[1] = keyFrame[minIndex].quaternion;
			quaternion[2] = keyFrame[maxIndex].quaternion;
			quaternion[3] = keyFrame[maxIndex + 1].quaternion;
		}

		result = Squad(quaternion[0], quaternion[1], quaternion[2], quaternion[3], float(time - minKeyFrame.time) / float(maxKeyFrame.time - minKeyFrame.time));
		break;
	default:
		break;
	}

	return result;
}