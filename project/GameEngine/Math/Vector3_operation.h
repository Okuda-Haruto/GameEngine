#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include <vector>

//加算
Vector3 Add(const Vector3& v1, const Vector3& v2);
//減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
//スカラー倍
Vector3 Multiply(float scalar, const Vector3& v);
//内積
float Dot(const Vector3& v1, const Vector3& v2);
//長さ(ノルム)
float Length(const Vector3& v);
//正規化
Vector3 Normalize(const Vector3& v);

//クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

// 線形補間
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);
Vector3 Bezier(const Vector3& v0, const Vector3& v1, const Vector3& v2, float t);
Vector3 Spline(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3, float t);
Vector3 Spline(const std::vector<Vector3>& cps, float t);

Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator-(const Vector3& v1, const Vector3& v2);
Vector3 operator*(float s, const Vector3& v);
Vector3 operator*(const Vector3& v, float s);
Vector3 operator/(const Vector3& v, float s);
Vector3 operator-(const Vector3& v);
Vector3 operator+(const Vector3& v);