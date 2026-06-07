#pragma once
#include <Vector3.h>
#include <Quaternion.h>
#include <string>

template <typename T>
struct Keyframe{
	float time;	//キーフレームの入っている時間
	T value;	//キーフレーム中の値
};

using Keyframe_Vector3 = Keyframe<Vector3>;
using Keyframe_Quaternion = Keyframe<Quaternion>;