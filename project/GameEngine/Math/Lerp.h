#pragma once
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>

// 線形補間
template <typename T>
T Lerp(T a, T b, float t) {
	T answer;
	answer = T(float(a) * (1 - t) + float(b) * t);
	return answer;
}