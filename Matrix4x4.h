#pragma once
#include "Vector3.h"

struct Matrix4x4 {
	float m[4][4];
};

Matrix4x4 MakeIdentity4x4();
Matrix4x4 Multipty(Matrix4x4 a, Matrix4x4 b);
Matrix4x4 Inverse(Matrix4x4 Matrix);
Matrix4x4 MakeScaleMatrix(Vector3 scale);
Matrix4x4 MakeTranslateMatrix(Vector3 translate);
Matrix4x4 MakeXRotateMatrix(float theta);
Matrix4x4 MakeYRotateMatrix(float theta);
Matrix4x4 MakeZRotateMatrix(float theta);
Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotate, Vector3 translate);
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);