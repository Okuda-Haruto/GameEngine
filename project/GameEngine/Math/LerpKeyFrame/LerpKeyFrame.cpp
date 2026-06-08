#include "LerpKeyFrame.h"
#include <Easing.h>
#include "Operation/Operation.h"
#include <cassert>

Vector3 LerpKeyFrame(std::vector<Keyframe_Vector3> keyframe, AnimationInterpolation interpolation, float time) {

	assert(!keyframe.empty());
	//キーが一つか、開始前なら最初のキーフレームの値
	if (keyframe.size() == 1 || time <= keyframe[0].time) {
		return keyframe[0].value;
	}

	Vector3 result;
	for (size_t index = 0; index < keyframe.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		if (keyframe[index].time <= time && time <= keyframe[nextIndex].time) {
			float t = (time - keyframe[index].time) / (keyframe[nextIndex].time - keyframe[index].time);

			//範囲内を補完
			switch (interpolation)
			{
			case AnimationInterpolation::Linear:
				result = Lerp(keyframe[index].value, keyframe[nextIndex].value, t);
				return result;
				break;
			case AnimationInterpolation::Step:
				if (time - keyframe[index].time <= keyframe[nextIndex].time - time) {
					result = keyframe[index].value;
				}
				else {
					result = keyframe[nextIndex].value;
				}
				return result;
				break;
			case AnimationInterpolation::Cubic_Spline:
				Vector3 vector[4];
				if (index <= 0) {
					vector[0] = keyframe[index].value;
					vector[1] = keyframe[index].value;
					vector[2] = keyframe[nextIndex].value;
					vector[3] = keyframe[nextIndex + 1].value;
				}
				else if (nextIndex >= keyframe.size() - 1) {
					vector[0] = keyframe[index - 1].value;
					vector[1] = keyframe[index].value;
					vector[2] = keyframe[nextIndex].value;
					vector[3] = keyframe[nextIndex].value;
				}
				else {
					vector[0] = keyframe[index - 1].value;
					vector[1] = keyframe[index].value;
					vector[2] = keyframe[nextIndex].value;
					vector[3] = keyframe[nextIndex + 1].value;
				}

				result = Spline(vector[0], vector[1], vector[2], vector[3], t);
				return result;
				break;
			default:
				break;
			}
		}
	}

	//ここまで来たら最後の値を返す
	return (*keyframe.rbegin()).value;
}

Quaternion LerpKeyFrame(std::vector<Keyframe_Quaternion> keyframe, AnimationInterpolation interpolation, float time) {

	assert(!keyframe.empty());
	//キーが一つか、開始前なら最初のキーフレームの値
	if (keyframe.size() == 1 || time <= keyframe[0].time) {
		return keyframe[0].value;
	}

	Quaternion result;
	for (size_t index = 0; index < keyframe.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		if (keyframe[index].time <= time && time <= keyframe[nextIndex].time) {
			float t = (time - keyframe[index].time) / (keyframe[nextIndex].time - keyframe[index].time);

			//範囲内を補完
			switch (interpolation)
			{
			case AnimationInterpolation::Linear:
				result = Slerp(keyframe[index].value, keyframe[nextIndex].value, t);
				return result;
				break;
			case AnimationInterpolation::Step:
				if (time - keyframe[index].time <= keyframe[nextIndex].time - time) {
					result = keyframe[index].value;
				}
				else {
					result = keyframe[nextIndex].value;
				}
				return result;
				break;
			case AnimationInterpolation::Cubic_Spline:
				Quaternion quaternion[4];
				if (index <= 0) {
					quaternion[0] = keyframe[index].value;
					quaternion[1] = keyframe[index].value;
					quaternion[2] = keyframe[nextIndex].value;
					quaternion[3] = keyframe[nextIndex + 1].value;
				}
				else if (nextIndex >= keyframe.size() - 1) {
					quaternion[0] = keyframe[index - 1].value;
					quaternion[1] = keyframe[index].value;
					quaternion[2] = keyframe[nextIndex].value;
					quaternion[3] = keyframe[nextIndex].value;
				}
				else {
					quaternion[0] = keyframe[index - 1].value;
					quaternion[1] = keyframe[index].value;
					quaternion[2] = keyframe[nextIndex].value;
					quaternion[3] = keyframe[nextIndex + 1].value;
				}

				result = Squad(quaternion[0], quaternion[1], quaternion[2], quaternion[3], t);
				return result;
				break;
			default:
				break;
			}
		}
	}

	//ここまで来たら最後の値を返す
	return (*keyframe.rbegin()).value;
}