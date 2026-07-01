#pragma once
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <SRT.h>
#include <cmath>

namespace Easing {

	// 線形補間
	template <typename T>
	inline T Lerp(T a, T b, float t) {
		T answer;
		answer = T(float(a) * (1.0f - t) + float(b) * t);
		return answer;
	}

	template<>
	inline Vector2 Lerp<Vector2>(Vector2 a, Vector2 b, float t) {
		Vector2 AnswerVector;
		AnswerVector.x = a.x * (1 - t) + b.x * t;
		AnswerVector.y = a.y * (1 - t) + b.y * t;
		return AnswerVector;
	}

	template<>
	inline Vector3 Lerp<Vector3>(Vector3 a, Vector3 b, float t) {
		Vector3 AnswerVector;
		AnswerVector.x = a.x * (1 - t) + b.x * t;
		AnswerVector.y = a.y * (1 - t) + b.y * t;
		AnswerVector.z = a.z * (1 - t) + b.z * t;
		return AnswerVector;
	}

	template<>
	inline Vector4 Lerp<Vector4>(Vector4 a, Vector4 b, float t) {
		Vector4 AnswerVector;
		AnswerVector.x = a.x * (1 - t) + b.x * t;
		AnswerVector.y = a.y * (1 - t) + b.y * t;
		AnswerVector.z = a.z * (1 - t) + b.z * t;
		AnswerVector.w = a.w * (1 - t) + b.w * t;
		return AnswerVector;
	}
	template<>
	inline SRT Lerp<SRT>(SRT a, SRT b, float t) {
		SRT AnswerTransform;
		AnswerTransform.scale = Lerp<Vector3>(a.scale, b.scale, t);
		AnswerTransform.rotate = Lerp<Vector3>(a.rotate, b.rotate, t);
		AnswerTransform.translate = Lerp<Vector3>(a.translate, b.translate, t);
		return AnswerTransform;
	}

	// イーズイン(加速)
	template <typename T>
	inline T EaseIn(T a, T b, float t) {
		return Lerp(a,b,powf(t,2));
	}

	// イーズアウト(減速)
	template <typename T>
	inline T EaseOut(T a, T b, float t) {
		return Lerp(a, b, 1.0f - powf(1.0f - t, 2));
	}

	// イーズインアウト(加減速)
	template <typename T>
	inline T EaseInOut(T a, T b, float t) {
		if (t < 0.5f) {
			return EaseIn(a, (a + b) / 2, t * 2);
		}
		else {
			return EaseOut((a + b) / 2, b, (t * 2) - 1.0f);
		}
	}
}