#include "Operation.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <cassert>

#pragma region Vector2

//加算
Vector2 Add(const Vector2& v1, const Vector2& v2) {
	Vector2 AnswerVector;
	AnswerVector.x = v1.x + v2.x;
	AnswerVector.y = v1.y + v2.y;
	return AnswerVector;
}
//減算
Vector2 Subtract(const Vector2& v1, const Vector2& v2) {
	Vector2 AnswerVector;
	AnswerVector.x = v1.x - v2.x;
	AnswerVector.y = v1.y - v2.y;
	return AnswerVector;
}
//スカラー倍
Vector2 Multiply(float scalar, const Vector2& v) {
	Vector2 AnswerVector;
	AnswerVector.x = scalar - v.x;
	AnswerVector.y = scalar - v.y;
	return AnswerVector;
}
//内積
float Dot(const Vector2& v1, const Vector2& v2) {
	float AnswerFloat;
	AnswerFloat = v1.x * v2.x + v1.y * v2.y;
	return AnswerFloat;
}
//長さ(ノルム)
float Length(const Vector2& v) {
	float AnswerFloat;
	AnswerFloat = sqrtf(Dot(v, v));
	return AnswerFloat;
}
//正規化
Vector2 Normalize(const Vector2& v) {
	Vector2 AnswerVector;
	AnswerVector = Multiply(1.0f / Length(v), v);
	return AnswerVector;
}

Vector2 Lerp(Vector2 a, Vector2 b, float t) {
	Vector2 AnswerVector;
	AnswerVector.x = a.x * (1 - t) + b.x * t;
	AnswerVector.y = a.y * (1 - t) + b.y * t;
	return AnswerVector;
}

Vector2 operator+(const Vector2& v1, const Vector2& v2) { return Add(v1, v2); }
Vector2 operator-(const Vector2& v1, const Vector2& v2) { return Subtract(v1, v2); }
Vector2 operator*(float s, const Vector2& v) { return Multiply(s, v); }
Vector2 operator*(const Vector2& v, float s) { return s * v; }
Vector2 operator/(const Vector2& v, float s) { return Multiply(1.0f /s, v); }
Vector2 operator-(const Vector2& v) { return { -v.x,-v.y }; }
Vector2 operator+(const Vector2& v) { return v; }

#pragma endregion

#pragma region Vector3

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
	AnswerFloat = sqrtf(Dot(v, v));
	return AnswerFloat;
}
//正規化
Vector3 Normalize(const Vector3& v) {
	Vector3 AnswerVector;
	AnswerVector = Multiply(1.0f / Length(v), v);
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

#pragma endregion

#pragma region Vector4

//加算
Vector4 Add(const Vector4& v1, const Vector4& v2) {
	Vector4 AnswerVector;
	AnswerVector.x = v1.x + v2.x;
	AnswerVector.y = v1.y + v2.y;
	AnswerVector.z = v1.z + v2.z;
	AnswerVector.w = v1.w + v2.w;
	return AnswerVector;
}
//減算
Vector4 Subtract(const Vector4& v1, const Vector4& v2) {
	Vector4 AnswerVector;
	AnswerVector.x = v1.x - v2.x;
	AnswerVector.y = v1.y - v2.y;
	AnswerVector.z = v1.z - v2.z;
	AnswerVector.w = v1.w - v2.w;
	return AnswerVector;
}
//スカラー倍
Vector4 Multiply(float scalar, const Vector4& v) {
	Vector4 AnswerVector;
	AnswerVector.x = scalar - v.x;
	AnswerVector.y = scalar - v.y;
	AnswerVector.z = scalar - v.z;
	AnswerVector.w = scalar - v.w;
	return AnswerVector;
}
//内積
float Dot(const Vector4& v1, const Vector4& v2) {
	float AnswerFloat;
	AnswerFloat = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
	return AnswerFloat;
}
//長さ(ノルム)
float Length(const Vector4& v) {
	float AnswerFloat;
	AnswerFloat = sqrtf(Dot(v, v));
	return AnswerFloat;
}
//正規化
Vector4 Normalize(const Vector4& v) {
	Vector4 AnswerVector;
	AnswerVector = Multiply(1.0f / Length(v), v);
	return AnswerVector;
}

//正規化
Vector4 NormalizeWeights(const Vector4& v) {
	float sum = v.x + v.y + v.z + v.w;
	if (sum > 0.0f) {
		return Vector4(v.x / sum, v.y / sum, v.z / sum, v.w / sum);
	}
	return v;
}

Vector4 Lerp(Vector4 a, Vector4 b, float t) {
	Vector4 AnswerVector;
	AnswerVector.x = a.x * (1 - t) + b.x * t;
	AnswerVector.y = a.y * (1 - t) + b.y * t;
	AnswerVector.z = a.z * (1 - t) + b.z * t;
	AnswerVector.w = a.w * (1 - t) + b.w * t;
	return AnswerVector;
}

Vector4 operator+(const Vector4& v1, const Vector4& v2) { return Add(v1, v2); }
Vector4 operator-(const Vector4& v1, const Vector4& v2) { return Subtract(v1, v2); }
Vector4 operator*(float s, const Vector4& v) { return Multiply(s, v); }
Vector4 operator*(const Vector4& v, float s) { return s * v; }
Vector4 operator/(const Vector4& v, float s) { return Multiply(1.0f / s, v); }
Vector4 operator-(const Vector4& v) { return { -v.x,-v.y }; }
Vector4 operator+(const Vector4& v) { return v; }

#pragma endregion

#pragma region Quaternion

Quaternion Add(const Quaternion& q0, const Quaternion& q1) {
	Quaternion returnQuaternion;
	returnQuaternion.x = q0.x + q1.x;
	returnQuaternion.y = q0.y + q1.y;
	returnQuaternion.z = q0.z + q1.z;
	returnQuaternion.w = q0.w + q1.w;
	return returnQuaternion;
}

Quaternion Subtract(const Quaternion& q0, const Quaternion& q1) {
	Quaternion returnQuaternion;
	returnQuaternion.x = q0.x - q1.x;
	returnQuaternion.y = q0.y - q1.y;
	returnQuaternion.z = q0.z - q1.z;
	returnQuaternion.w = q0.w - q1.w;
	return returnQuaternion;
}

Quaternion Multiply(const float& f, const Quaternion& q) {
	Quaternion returnQuaternion;
	returnQuaternion.x = q.x * f;
	returnQuaternion.y = q.y * f;
	returnQuaternion.z = q.z * f;
	returnQuaternion.w = q.w * f;
	return returnQuaternion;
}


Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs) {
	Quaternion returnQuaternion;
	Vector3 qv = { lhs.x,lhs.y,lhs.z };
	Vector3 rv = { rhs.x,rhs.y,rhs.z };

	returnQuaternion.w = lhs.w * rhs.w - Dot(qv, rv);
	Vector3 ansQ = Cross(qv, rv) + qv * rhs.w + rv * lhs.w;
	returnQuaternion.x = ansQ.x;
	returnQuaternion.y = ansQ.y;
	returnQuaternion.z = ansQ.z;

	return returnQuaternion;
}

Quaternion IdentityQuaternion() {
	Quaternion returnQuaternion{};

	returnQuaternion.w = 1.0f;

	return returnQuaternion;
}

Quaternion Conjugate(const Quaternion& quaternion) {
	Quaternion returnQuaternion{};

	returnQuaternion.x = -quaternion.x;
	returnQuaternion.y = -quaternion.y;
	returnQuaternion.z = -quaternion.z;
	returnQuaternion.w = quaternion.w;

	return returnQuaternion;
}

float Norm(const Quaternion& quaternion) {
	float norm;

	norm = sqrtf(powf(quaternion.w, 2) + powf(quaternion.x, 2) + powf(quaternion.y, 2) + powf(quaternion.z, 2));

	return norm;
}

Quaternion Normalize(const Quaternion& quaternion) {
	Quaternion returnQuaternion;
	float norm = Norm(quaternion);

	returnQuaternion.x = quaternion.x / norm;
	returnQuaternion.y = quaternion.y / norm;
	returnQuaternion.z = quaternion.z / norm;
	returnQuaternion.w = quaternion.w / norm;

	return returnQuaternion;
}

Quaternion Inverse(const Quaternion& quaternion) {
	Quaternion returnQuaternion;
	Quaternion conjugate = Conjugate(quaternion);
	float norm = Norm(quaternion);

	returnQuaternion.x = conjugate.x / powf(norm, 2);
	returnQuaternion.y = conjugate.y / powf(norm, 2);
	returnQuaternion.z = conjugate.z / powf(norm, 2);
	returnQuaternion.w = conjugate.w / powf(norm, 2);

	return returnQuaternion;
}

Quaternion Log(const Quaternion& quaternion) {
	Quaternion returnQuaternion;

	Vector3 v = Vector3(quaternion.x, quaternion.y, quaternion.z);

	// ノルムがゼロに近い場合は 0 を返す
	if (Length(v) < 1e-6f) {
		returnQuaternion.x = returnQuaternion.y = returnQuaternion.z = 0.0f;
		returnQuaternion.w = 0.0f;
		return returnQuaternion;
	}

	float theta = std::atan2(Length(v), quaternion.w);
	float inv = theta / Length(v);

	returnQuaternion = { quaternion.x * inv, quaternion.y * inv, quaternion.z * inv,0.0f };
	return returnQuaternion;
}

Quaternion Exp(const Quaternion& quaternion) {
	Quaternion returnQuaternion;

	Vector3 v = Vector3(quaternion.x, quaternion.y, quaternion.z);

	// |v| が 0 のとき，exp(0) = identity
	if (Length(v) < 1e-6f) {
		returnQuaternion.x = returnQuaternion.y = returnQuaternion.z = 0.0f;
		returnQuaternion.w = 1.0f;
		return returnQuaternion;
	}

	float sin = std::sinf(Length(v));
	float cos = std::cosf(Length(v));
	float inv = sin / Length(v);

	returnQuaternion = { quaternion.x * inv, quaternion.y * inv, quaternion.z * inv,cos };
	return returnQuaternion;
}

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	Quaternion returnQuaternion;

	returnQuaternion.w = cosf(angle / 2.0f);
	Vector3 vec = axis * sinf(angle / 2.0f);
	returnQuaternion.x = vec.x;
	returnQuaternion.y = vec.y;
	returnQuaternion.z = vec.z;

	return returnQuaternion;
}

Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion) {
	Quaternion vecQuaternion;
	vecQuaternion.x = vector.x;
	vecQuaternion.y = vector.y;
	vecQuaternion.z = vector.z;
	vecQuaternion.w = 0.0f;

	vecQuaternion = Multiply(Multiply(quaternion, vecQuaternion), Conjugate(quaternion));

	Vector3 returnVector;
	returnVector.x = vecQuaternion.x;
	returnVector.y = vecQuaternion.y;
	returnVector.z = vecQuaternion.z;

	return returnVector;
}

float Dot(const Quaternion& q0, const Quaternion& q1) {
	return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) {
	Quaternion Q0 = q0, Q1 = q1;
	float dot = Dot(Q0, Q1);
	if (dot < 0) {
		Q0 = -Q0;
		dot = -dot;
	}
	dot = std::clamp(dot, -1.0f, 1.0f);

	//ほとんど1.0fならすぐに返す
	const float EPS = 1e-5f;
	if (dot > 1.0f - EPS) {
		return Normalize((1 - t) * Q0 + t * Q1);
	}

	float theta = std::acosf(dot);
	if (theta == 0) {
		return q0;
	}

	float scale0 = std::sinf((1 - t) * theta) / std::sinf(theta);
	float scale1 = std::sinf(t * theta) / std::sinf(theta);

	return scale0 * Q0 + scale1 * Q1;
}


Quaternion Squad(const Quaternion& q0, const Quaternion& q1,
	const Quaternion& q2, const Quaternion& q3, float t)
{
	// t を 0〜1 に制限
	t = std::clamp(t, 0.0f, 1.0f);

	// q1 の制御点 s1 を計算
	Quaternion invQ1 = Inverse(q1);
	Quaternion log1 = Log(invQ1 * q2);
	Quaternion log2 = Log(invQ1 * q0);
	Quaternion s1 = q1 * Exp((log1 + log2) * -0.25f);

	// q2 の制御点 s2 を計算
	Quaternion invQ2 = Inverse(q2);
	log1 = Log(invQ2 * q3);
	log2 = Log(invQ2 * q1);
	Quaternion s2 = q2 * Exp((log1 + log2) * -0.25f);

	// 内側の SLERP
	Quaternion slerp1 = Slerp(q1, q2, t);
	Quaternion slerp2 = Slerp(s1, s2, t);

	// 外側の SLERP
	return Slerp(slerp1, slerp2, 2.0f * t * (1.0f - t));
}
Quaternion operator+(const Quaternion& q1, const Quaternion& q2) { return Add(q1, q2); }
Quaternion operator-(const Quaternion& q1, const Quaternion& q2) { return Subtract(q1, q2); }
Quaternion operator*(float s, const Quaternion& q) { return Multiply(s, q); }
Quaternion operator*(const Quaternion& q, float s) { return s * q; }
Quaternion operator*(const Quaternion& q1, Quaternion q2) { return Multiply(q1, q2); }
Quaternion operator/(const Quaternion& q, float s) { return (1 / s) * q; }
Quaternion operator-(const Quaternion& q) { return Quaternion{ -q.x,-q.y,-q.z,-q.w }; }
Quaternion operator+(const Quaternion& q) { return q; }

#pragma endregion

#pragma region Matrix4x4

//行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 returnMatrix;
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			returnMatrix.m[row][column] = m1.m[row][column] + m2.m[row][column];
		}
	}
	return returnMatrix;
}
//行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 returnMatrix;
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			returnMatrix.m[row][column] = m1.m[row][column] - m2.m[row][column];
		}
	}
	return returnMatrix;
}
//行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 returnMatrix;
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			returnMatrix.m[row][column] = m1.m[row][0] * m2.m[0][column] + m1.m[row][1] * m2.m[1][column] + m1.m[row][2] * m2.m[2][column] + m1.m[row][3] * m2.m[3][column];
		}
	}
	return returnMatrix;
}
//逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	float determinant = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]
		- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]
		+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]
		+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]) / determinant;
	returnMatrix.m[0][1] = (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]) / determinant;
	returnMatrix.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]) / determinant;
	returnMatrix.m[0][3] = (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]) / determinant;

	returnMatrix.m[1][0] = (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]) / determinant;
	returnMatrix.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]) / determinant;
	returnMatrix.m[1][2] = (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]) / determinant;
	returnMatrix.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]) / determinant;

	returnMatrix.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]) / determinant;
	returnMatrix.m[2][1] = (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]) / determinant;
	returnMatrix.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]) / determinant;
	returnMatrix.m[2][3] = (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]) / determinant;

	returnMatrix.m[3][0] = (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]) / determinant;
	returnMatrix.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]) / determinant;
	returnMatrix.m[3][2] = (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]) / determinant;
	returnMatrix.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]) / determinant;
	return returnMatrix;
}
//転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 returnMatrix;
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			returnMatrix.m[row][column] = m.m[column][row];
		}
	}
	return returnMatrix;
}
//単位表列の作成
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = 1.0f; returnMatrix.m[0][1] = 0.0f; returnMatrix.m[0][2] = 0.0f; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 0.0f; returnMatrix.m[1][1] = 1.0f; returnMatrix.m[1][2] = 0.0f; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 0.0f; returnMatrix.m[2][1] = 0.0f; returnMatrix.m[2][2] = 1.0f; returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = 0.0f; returnMatrix.m[3][1] = 0.0f; returnMatrix.m[3][2] = 0.0f; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

//平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = 1.0f; returnMatrix.m[0][1] = 0.0f; returnMatrix.m[0][2] = 0.0f; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 0.0f; returnMatrix.m[1][1] = 1.0f; returnMatrix.m[1][2] = 0.0f; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 0.0f; returnMatrix.m[2][1] = 0.0f; returnMatrix.m[2][2] = 1.0f; returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = translate.x; returnMatrix.m[3][1] = translate.y; returnMatrix.m[3][2] = translate.z; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

//拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = scale.x; returnMatrix.m[0][1] = 0.0f; returnMatrix.m[0][2] = 0.0f; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 0.0f; returnMatrix.m[1][1] = scale.y; returnMatrix.m[1][2] = 0.0f; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 0.0f; returnMatrix.m[2][1] = 0.0f; returnMatrix.m[2][2] = scale.z; returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = 0.0f; returnMatrix.m[3][1] = 0.0f; returnMatrix.m[3][2] = 0.0f; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

//座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 returnVector;
	returnVector.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	returnVector.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	returnVector.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	returnVector.x /= w;
	returnVector.y /= w;
	returnVector.z /= w;
	return returnVector;
}

//X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = 1.0f; returnMatrix.m[0][1] = 0.0f; returnMatrix.m[0][2] = 0.0f; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 0.0f; returnMatrix.m[1][1] = std::cos(radian); returnMatrix.m[1][2] = std::sin(radian); returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 0.0f; returnMatrix.m[2][1] = -std::sin(radian); returnMatrix.m[2][2] = std::cos(radian); returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = 0.0f; returnMatrix.m[3][1] = 0.0f; returnMatrix.m[3][2] = 0.0f; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

//Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = std::cos(radian); returnMatrix.m[0][1] = 0.0f; returnMatrix.m[0][2] = -std::sin(radian); returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 0.0f; returnMatrix.m[1][1] = 1.0f; returnMatrix.m[1][2] = 0.0f; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = std::sin(radian); returnMatrix.m[2][1] = 0.0f; returnMatrix.m[2][2] = std::cos(radian); returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = 0.0f; returnMatrix.m[3][1] = 0.0f; returnMatrix.m[3][2] = 0.0f; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

//Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = std::cos(radian); returnMatrix.m[0][1] = std::sin(radian); returnMatrix.m[0][2] = 0.0f; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = -std::sin(radian); returnMatrix.m[1][1] = std::cos(radian); returnMatrix.m[1][2] = 0.0f; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 0.0f; returnMatrix.m[2][1] = 0.0f; returnMatrix.m[2][2] = 1.0f; returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = 0.0f; returnMatrix.m[3][1] = 0.0f; returnMatrix.m[3][2] = 0.0f; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

//回転行列
Matrix4x4 MakeRotateMatrix(Vector3 rotate) {
	Matrix4x4 returnMatrix;
	returnMatrix = MakeRotateXMatrix(rotate.x) * MakeRotateYMatrix(rotate.y) * MakeRotateZMatrix(rotate.z);
	return returnMatrix;
}

//任意軸回転行列
Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle) {
	Matrix4x4 returnMatrix;

	Matrix4x4 S = MakeScaleMatrix(Vector3{ std::cosf(angle),std::cosf(angle) ,std::cosf(angle) });

	Matrix4x4 P = (1.0f - std::cosf(angle)) * DotMatrix(axis, axis);

	Matrix4x4 C = -std::sinf(angle) * CrossMatrix(axis);

	returnMatrix = Transpose(S + P - C);
	return returnMatrix;
}

//Quaternionからの回転行列
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion) {
	Matrix4x4 returnMatrix;

	returnMatrix.m[0][0] = 1.0f - 2.0f * (quaternion.y * quaternion.y + quaternion.z * quaternion.z);	returnMatrix.m[0][1] = 2.0f * (quaternion.x * quaternion.y - quaternion.z * quaternion.w);				returnMatrix.m[0][2] = 2.0f * (quaternion.x * quaternion.z + quaternion.y * quaternion.w);			returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 2.0f * (quaternion.x * quaternion.y + quaternion.z * quaternion.w);			returnMatrix.m[1][1] = 1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.z * quaternion.z);		returnMatrix.m[1][2] = 2.0f * (quaternion.y * quaternion.z - quaternion.x * quaternion.w);			returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 2.0f * (quaternion.x * quaternion.z - quaternion.y * quaternion.w);			returnMatrix.m[2][1] = 2.0f * (quaternion.y * quaternion.z + quaternion.x * quaternion.w);				returnMatrix.m[2][2] = 1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.y * quaternion.y);	returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = 0.0f;																		returnMatrix.m[3][1] = 0.0f;																			returnMatrix.m[3][2] = 0.0f;																		returnMatrix.m[3][3] = 1.0f;

	return returnMatrix;
}


//3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotate, Vector3 translate) {

	Matrix4x4 rotateXMatrix = MakeRotateAxisAngle(Vector3{ 1.0f,0.0f,0.0f }, rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateAxisAngle(Vector3{ 0.0f,1.0f,0.0f }, rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateAxisAngle(rotateYMatrix * Vector3{ 0.0f,0.0f,1.0f }, rotate.z);

	Matrix4x4 rotateMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;

	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = scale.x * rotateMatrix.m[0][0]; returnMatrix.m[0][1] = scale.x * rotateMatrix.m[0][1]; returnMatrix.m[0][2] = scale.x * rotateMatrix.m[0][2]; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = scale.y * rotateMatrix.m[1][0]; returnMatrix.m[1][1] = scale.y * rotateMatrix.m[1][1]; returnMatrix.m[1][2] = scale.y * rotateMatrix.m[1][2]; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = scale.z * rotateMatrix.m[2][0]; returnMatrix.m[2][1] = scale.z * rotateMatrix.m[2][1]; returnMatrix.m[2][2] = scale.z * rotateMatrix.m[2][2]; returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = translate.x; returnMatrix.m[3][1] = translate.y; returnMatrix.m[3][2] = translate.z; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

//クォータニオン行列
Matrix4x4 MakeQuaternionMatrix(Vector3 scale, Quaternion rotate, Vector3 translate) {

	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotate);

	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = scale.x * rotateMatrix.m[0][0]; returnMatrix.m[0][1] = scale.x * rotateMatrix.m[0][1]; returnMatrix.m[0][2] = scale.x * rotateMatrix.m[0][2]; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = scale.y * rotateMatrix.m[1][0]; returnMatrix.m[1][1] = scale.y * rotateMatrix.m[1][1]; returnMatrix.m[1][2] = scale.y * rotateMatrix.m[1][2]; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = scale.z * rotateMatrix.m[2][0]; returnMatrix.m[2][1] = scale.z * rotateMatrix.m[2][1]; returnMatrix.m[2][2] = scale.z * rotateMatrix.m[2][2]; returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = translate.x; returnMatrix.m[3][1] = translate.y; returnMatrix.m[3][2] = translate.z; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;

}

//内積行列
Matrix4x4 DotMatrix(const Vector3& v1, const Vector3& v2) {
	Matrix4x4 returnMatrix;

	returnMatrix.m[0][0] = v2.x * v1.x; returnMatrix.m[0][1] = v2.x * v1.y; returnMatrix.m[0][2] = v2.x * v1.z; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = v2.y * v1.x; returnMatrix.m[1][1] = v2.y * v1.y; returnMatrix.m[1][2] = v2.y * v1.z; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = v2.z * v1.x; returnMatrix.m[2][1] = v2.z * v1.y; returnMatrix.m[2][2] = v2.z * v1.z; returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = 0.0f;		returnMatrix.m[3][1] = 0.0f;		returnMatrix.m[3][2] = 0.0f;		returnMatrix.m[3][3] = 1.0f;

	return returnMatrix;
}

//クロス積行列
Matrix4x4 CrossMatrix(const Vector3& v) {
	Matrix4x4 returnMatrix;

	returnMatrix.m[0][0] = 0.0f; returnMatrix.m[0][1] = -v.z; returnMatrix.m[0][2] = v.y;  returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = v.z;  returnMatrix.m[1][1] = 0.0f; returnMatrix.m[1][2] = -v.x; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = -v.y; returnMatrix.m[2][1] = v.x;  returnMatrix.m[2][2] = 0.0f; returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = 0.0f; returnMatrix.m[3][1] = 0.0f;  returnMatrix.m[3][2] = 0.0f; returnMatrix.m[3][3] = 1.0f;

	return returnMatrix;
}

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = 1 / tanf(fovY / 2) / aspectRatio; returnMatrix.m[0][1] = 0.0f; returnMatrix.m[0][2] = 0.0f; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 0.0f; returnMatrix.m[1][1] = 1 / tanf(fovY / 2); returnMatrix.m[1][2] = 0.0f; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 0.0f; returnMatrix.m[2][1] = 0.0f; returnMatrix.m[2][2] = farClip / (farClip - nearClip); returnMatrix.m[2][3] = 1.0f;
	returnMatrix.m[3][0] = 0.0f; returnMatrix.m[3][1] = 0.0f; returnMatrix.m[3][2] = -nearClip * farClip / (farClip - nearClip); returnMatrix.m[3][3] = 0.0f;
	return returnMatrix;
}

//正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = 2 / (right - left); returnMatrix.m[0][1] = 0.0f; returnMatrix.m[0][2] = 0.0f; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 0.0f; returnMatrix.m[1][1] = 2 / (top - bottom); returnMatrix.m[1][2] = 0.0f; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 0.0f; returnMatrix.m[2][1] = 0.0f; returnMatrix.m[2][2] = 1 / (farClip - nearClip); returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = (left + right) / (left - right); returnMatrix.m[3][1] = (top + bottom) / (bottom - top); returnMatrix.m[3][2] = nearClip / (nearClip - farClip); returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = width / 2; returnMatrix.m[0][1] = 0.0f; returnMatrix.m[0][2] = 0.0f; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 0.0f; returnMatrix.m[1][1] = -height / 2; returnMatrix.m[1][2] = 0.0f; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 0.0f; returnMatrix.m[2][1] = 0.0f; returnMatrix.m[2][2] = maxDepth - minDepth; returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = left + width / 2; returnMatrix.m[3][1] = top + height / 2; returnMatrix.m[3][2] = minDepth; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

//aiMatrix変換(GLTF用)
Matrix4x4 aiMatrix4x4ToMatrix4x4(aiMatrix4x4 matrix) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = matrix.a1; returnMatrix.m[0][1] = matrix.a2; returnMatrix.m[0][2] = matrix.a3; returnMatrix.m[0][3] = matrix.a4;
	returnMatrix.m[1][0] = matrix.b1; returnMatrix.m[1][1] = matrix.b2; returnMatrix.m[1][2] = matrix.b3; returnMatrix.m[1][3] = matrix.b4;
	returnMatrix.m[2][0] = matrix.c1; returnMatrix.m[2][1] = matrix.c2; returnMatrix.m[2][2] = matrix.c3; returnMatrix.m[2][3] = matrix.c4;
	returnMatrix.m[3][0] = matrix.d1; returnMatrix.m[3][1] = matrix.d2; returnMatrix.m[3][2] = matrix.d3; returnMatrix.m[3][3] = matrix.d4;
	return returnMatrix;
}

Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) { return Add(m1, m2); }
Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) { return Subtract(m1, m2); }
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) { return Multiply(m1, m2); }
Matrix4x4 operator*(const Matrix4x4& m, const float f) {
	Matrix4x4 returnMatrix;
	returnMatrix = m;
	for (int row = 0; row < 3; row++) {
		for (int column = 0; column < 3; column++) {
			returnMatrix.m[row][column] *= f;
		}
	}
	return returnMatrix;
};
Matrix4x4 operator*(const float f, const Matrix4x4& m) { return m * f; }
Vector3 operator*(const Vector3& v, const Matrix4x4& m) { return Transform(v, m); }
Vector3 operator*(const Matrix4x4& m, const Vector3& v) { return v * m; }

#pragma endregion