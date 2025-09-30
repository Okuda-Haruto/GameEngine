#include "Vector3_operation.h"
#include <cmath>
#include <iostream>
#include <algorithm>

//加算
Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 AnswerVector;
	AnswerVector.x = v1.x + v2.x;
	AnswerVector.y = v1.y + v2.y;
	AnswerVector.z = v1.z + v2.z;
	return AnswerVector;
}
//減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 AnswerVector;
	AnswerVector.x = v1.x - v2.x;
	AnswerVector.y = v1.y - v2.y;
	AnswerVector.z = v1.z - v2.z;
	return AnswerVector;
}
//スカラー倍
Vector3 Multiply(float scalar, const Vector3& v) {
	Vector3 AnswerVector;
	AnswerVector.x = scalar * v.x;
	AnswerVector.y = scalar * v.y;
	AnswerVector.z = scalar * v.z;
	return AnswerVector;
}
//内積
float Dot(const Vector3& v1, const Vector3& v2) {
	float AnswerFloat;
	AnswerFloat = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return AnswerFloat;
}
//長さ(ノルム)
float Length(const Vector3& v) {
	float AnswerFloat;
	AnswerFloat = sqrtf(Dot(v,v));
	return AnswerFloat;
}
//正規化
Vector3 Normalize(const Vector3& v) {
	Vector3 AnswerVector;
	AnswerVector = Multiply(1.0f / Length(v),v);
	return AnswerVector;
}

//クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 AnswerVector;
	AnswerVector.x = v1.y * v2.z - v1.z * v2.y;
	AnswerVector.y = v1.z * v2.x - v1.x * v2.z;
	AnswerVector.z = v1.x * v2.y - v1.y * v2.x;
	return AnswerVector;
}

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) {
	Vector3 result{
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] };

	return result;
}


// 線形補間
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
	Vector3 AnswerVector;
	AnswerVector.x = v1.x * (1 - t) + v2.x * t;
	AnswerVector.y = v1.y * (1 - t) + v2.y * t;
	AnswerVector.z = v1.z * (1 - t) + v2.z * t;
	return AnswerVector;
}

Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t) {

	Vector3 v1normal = Normalize(v1);
	Vector3 v2normal = Normalize(v2);

	//内積を求める
	float dot = Dot(v1normal, v2normal);

	//誤差により1.0fを超えるのを防ぐ
	dot = std::min(dot, 1.0f);
	//アークコサインでθの角度を求める
	float theta = std::acos(dot);
	//θの角度からsinθを求める
	float sinTheta = std::sin(theta);
	//サイン(θ(1-t))を求める
	float sinThetaFrom = std::sin((1 - t) * theta);
	//サインθtを求める
	float sinThetaTo = std::sin(t * theta);

	//球面線形補間したベクトル(単位ベクトル)
	Vector3 lerpNormal;
	if (sinTheta < 1.0e-5) {
		lerpNormal = v1normal;
	} else {
		lerpNormal = Multiply(1 / sinTheta, Add(Multiply(sinThetaFrom, v1normal), Multiply(sinThetaTo, v2normal)));
	}

	//ベクトルの長さはv1とv2の長さを線形補間
	float length1 = Length(v1);
	float length2 = Length(v2);
	//Lerpで補完ベクトルの長さを求める
	float length = std::lerp(length1, length2, t);

	//長さを反映
	return Multiply(length, lerpNormal);
}

Vector3 Bezier(const Vector3& v0, const Vector3& v1, const Vector3& v2, float t) {
	Vector3 p0p1 = Lerp(v0, v1, t);
	Vector3 p1p2 = Lerp(v1, v2, t);
	Vector3 p = Lerp(p0p1, p1p2, t);
	return p;
}

Vector3 Spline(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3, float t) {
	Vector3 p;

	// 補間範囲を0から1に限定
	t = std::clamp(t, 0.0f, 1.0f);

	//原点以外から始めるとズレるので原点に移動
	Vector3 p0{};
	Vector3 p1 = v1 - v0;
	Vector3 p2 = v2 - v0;
	Vector3 p3 = v3 - v0;

	p = (
		(-p0 + (3 * p1) - (3 * p2) + p3) * powf(t, 3) +
		((2 * p0) - (5 * p1) + (4 * p2) - p3) * powf(t, 2) +
		(p0 + p2) * t +
		2 * p1
		) / 2;

	//原点から元の位置に戻す
	return p + v0;
}

Vector3 Spline(const std::vector<Vector3>& controlPoint, float t) {

	float divisionPoint = 1.0f / controlPoint.size();
	int drawPoint = int(t / divisionPoint);

	Vector3 p;
	if (drawPoint > controlPoint.size() - 2) { // 存在しない通過点を通らないようにする
		return p;
	}

	if (drawPoint <= 0) {
		p = Spline(controlPoint[0], controlPoint[0], controlPoint[1], controlPoint[2], t / divisionPoint - float(drawPoint));
	} else if (drawPoint == controlPoint.size() - 2) {	//存在しない通過点を通らないようにする
		p = Spline(controlPoint[controlPoint.size() - 3], controlPoint[controlPoint.size() - 2], controlPoint[controlPoint.size() - 1], controlPoint[controlPoint.size() - 1], t / divisionPoint - float(drawPoint));
	} else {
		p = Spline(controlPoint[drawPoint - 1], controlPoint[drawPoint], controlPoint[drawPoint + 1], controlPoint[drawPoint + 2], t / divisionPoint - float(drawPoint));
	}

	return p;
}

Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Add(v1, v2); }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Subtract(v1, v2); }
Vector3 operator*(float s, const Vector3& v) { return Multiply(s, v); }
Vector3 operator*(const Vector3& v, float s) { return s * v; }
Vector3 operator/(const Vector3& v, float s) { return Multiply(1.0f / s, v); }
Vector3 operator-(const Vector3& v) { return{ -v.x,-v.y,-v.z }; }
Vector3 operator+(const Vector3& v) { return v; }