#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include <vector>

// 線形補間
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);
Vector3 Bezier(const Vector3& v0, const Vector3& v1, const Vector3& v2, float t);
Vector3 Spline(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3, float t);
Vector3 Spline(const std::vector<Vector3>& cps, float t);