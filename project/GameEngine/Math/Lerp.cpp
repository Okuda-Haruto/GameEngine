#include <Lerp.h>

template<>
Vector2 Lerp<Vector2>(Vector2 a, Vector2 b, float t) {
	Vector2 AnswerVector;
	AnswerVector.x = a.x * (1 - t) + b.x * t;
	AnswerVector.y = a.y * (1 - t) + b.y * t;
	return AnswerVector;
}

template<>
Vector3 Lerp<Vector3>(Vector3 a, Vector3 b, float t) {
	Vector3 AnswerVector;
	AnswerVector.x = a.x * (1 - t) + b.x * t;
	AnswerVector.y = a.y * (1 - t) + b.y * t;
	AnswerVector.z = a.z * (1 - t) + b.z * t;
	return AnswerVector;
}

template<>
Vector4 Lerp<Vector4>(Vector4 a, Vector4 b, float t) {
	Vector4 AnswerVector;
	AnswerVector.x = a.x * (1 - t) + b.x * t;
	AnswerVector.y = a.y * (1 - t) + b.y * t;
	AnswerVector.z = a.z * (1 - t) + b.z * t;
	AnswerVector.w = a.w * (1 - t) + b.w * t;
	return AnswerVector;
}