#include "Matrix4x4.h"
#define _USE_MATH_DEFINES
#include <math.h>

Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = 1.0f; returnMatrix.m[0][1] = 0.0f; returnMatrix.m[0][2] = 0.0f; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 0.0f; returnMatrix.m[1][1] = 1.0f; returnMatrix.m[1][2] = 0.0f; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 0.0f; returnMatrix.m[2][1] = 0.0f; returnMatrix.m[2][2] = 1.0f; returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = 0.0f; returnMatrix.m[3][1] = 0.0f; returnMatrix.m[3][2] = 0.0f; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

Matrix4x4 Multipty(Matrix4x4 a, Matrix4x4 b) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0] + a.m[0][2] * b.m[2][0] + a.m[0][3] * b.m[3][0]; returnMatrix.m[0][1] = a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1] + a.m[0][2] * b.m[2][1] + a.m[0][3] * b.m[3][1]; returnMatrix.m[0][2] = a.m[0][0] * b.m[0][2] + a.m[0][1] * b.m[1][2] + a.m[0][2] * b.m[2][2] + a.m[0][3] * b.m[3][2]; returnMatrix.m[0][3] = a.m[0][0] * b.m[0][3] + a.m[0][1] * b.m[1][3] + a.m[0][2] * b.m[2][3] + a.m[0][3] * b.m[3][3];
	returnMatrix.m[1][0] = a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0] + a.m[1][2] * b.m[2][0] + a.m[1][3] * b.m[3][0]; returnMatrix.m[1][1] = a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1] + a.m[1][2] * b.m[2][1] + a.m[1][3] * b.m[3][1]; returnMatrix.m[1][2] = a.m[1][0] * b.m[0][2] + a.m[1][1] * b.m[1][2] + a.m[1][2] * b.m[2][2] + a.m[1][3] * b.m[3][2]; returnMatrix.m[1][3] = a.m[1][0] * b.m[0][3] + a.m[1][1] * b.m[1][3] + a.m[1][2] * b.m[2][3] + a.m[1][3] * b.m[3][3];
	returnMatrix.m[2][0] = a.m[2][0] * b.m[0][0] + a.m[2][1] * b.m[1][0] + a.m[2][2] * b.m[2][0] + a.m[2][3] * b.m[3][0]; returnMatrix.m[2][1] = a.m[2][0] * b.m[0][1] + a.m[2][1] * b.m[1][1] + a.m[2][2] * b.m[2][1] + a.m[2][3] * b.m[3][1]; returnMatrix.m[2][2] = a.m[2][0] * b.m[0][2] + a.m[2][1] * b.m[1][2] + a.m[2][2] * b.m[2][2] + a.m[2][3] * b.m[3][2]; returnMatrix.m[2][3] = a.m[2][0] * b.m[0][3] + a.m[2][1] * b.m[1][3] + a.m[2][2] * b.m[2][3] + a.m[2][3] * b.m[3][3];
	returnMatrix.m[3][0] = a.m[3][0] * b.m[0][0] + a.m[3][1] * b.m[1][0] + a.m[3][2] * b.m[2][0] + a.m[3][3] * b.m[3][0]; returnMatrix.m[3][1] = a.m[3][0] * b.m[0][1] + a.m[3][1] * b.m[1][1] + a.m[3][2] * b.m[2][1] + a.m[3][3] * b.m[3][1]; returnMatrix.m[3][2] = a.m[3][0] * b.m[0][2] + a.m[3][1] * b.m[1][2] + a.m[3][2] * b.m[2][2] + a.m[3][3] * b.m[3][2]; returnMatrix.m[3][3] = a.m[3][0] * b.m[0][3] + a.m[3][1] * b.m[1][3] + a.m[3][2] * b.m[2][3] + a.m[3][3] * b.m[3][3];
	return returnMatrix;
}

Matrix4x4 Inverse(Matrix4x4 Matrix) {
	float determinant = Matrix.m[0][0] * Matrix.m[1][1] * Matrix.m[2][2] * Matrix.m[3][3] + Matrix.m[0][0] * Matrix.m[1][2] * Matrix.m[2][3] * Matrix.m[3][1] + Matrix.m[0][0] * Matrix.m[1][3] * Matrix.m[2][1] * Matrix.m[3][2]
		- Matrix.m[0][0] * Matrix.m[1][3] * Matrix.m[2][2] * Matrix.m[3][1] - Matrix.m[0][0] * Matrix.m[1][2] * Matrix.m[2][1] * Matrix.m[3][3] - Matrix.m[0][0] * Matrix.m[1][1] * Matrix.m[2][3] * Matrix.m[3][2]
		- Matrix.m[0][1] * Matrix.m[1][0] * Matrix.m[2][2] * Matrix.m[3][3] - Matrix.m[0][2] * Matrix.m[1][0] * Matrix.m[2][3] * Matrix.m[3][1] - Matrix.m[0][3] * Matrix.m[1][0] * Matrix.m[2][1] * Matrix.m[3][2]
		+ Matrix.m[0][3] * Matrix.m[1][0] * Matrix.m[2][2] * Matrix.m[3][1] + Matrix.m[0][2] * Matrix.m[1][0] * Matrix.m[2][1] * Matrix.m[3][3] + Matrix.m[0][1] * Matrix.m[1][0] * Matrix.m[2][3] * Matrix.m[3][2]
		+ Matrix.m[0][1] * Matrix.m[1][2] * Matrix.m[2][0] * Matrix.m[3][3] + Matrix.m[0][2] * Matrix.m[1][3] * Matrix.m[2][0] * Matrix.m[3][1] + Matrix.m[0][3] * Matrix.m[1][1] * Matrix.m[2][0] * Matrix.m[3][2]
		- Matrix.m[0][3] * Matrix.m[1][2] * Matrix.m[2][0] * Matrix.m[3][1] - Matrix.m[0][2] * Matrix.m[1][1] * Matrix.m[2][0] * Matrix.m[3][3] - Matrix.m[0][1] * Matrix.m[1][3] * Matrix.m[2][0] * Matrix.m[3][2]
		- Matrix.m[0][1] * Matrix.m[1][2] * Matrix.m[2][3] * Matrix.m[3][0] - Matrix.m[0][2] * Matrix.m[1][3] * Matrix.m[2][1] * Matrix.m[3][0] - Matrix.m[0][3] * Matrix.m[1][1] * Matrix.m[2][2] * Matrix.m[3][0]
		+ Matrix.m[0][3] * Matrix.m[1][2] * Matrix.m[2][1] * Matrix.m[3][0] + Matrix.m[0][2] * Matrix.m[1][1] * Matrix.m[2][3] * Matrix.m[3][0] + Matrix.m[0][1] * Matrix.m[1][3] * Matrix.m[2][2] * Matrix.m[3][0];

	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] =  (Matrix.m[1][1] * Matrix.m[2][2] * Matrix.m[3][3] + Matrix.m[1][2] * Matrix.m[2][3] * Matrix.m[3][1] + Matrix.m[1][3] * Matrix.m[2][1] * Matrix.m[3][2] - Matrix.m[1][3] * Matrix.m[2][2] * Matrix.m[3][1] - Matrix.m[1][2] * Matrix.m[2][1] * Matrix.m[3][3] - Matrix.m[1][1] * Matrix.m[2][3] * Matrix.m[3][2]) / determinant;
	returnMatrix.m[0][1] = (-Matrix.m[0][1] * Matrix.m[2][2] * Matrix.m[3][3] - Matrix.m[0][2] * Matrix.m[2][3] * Matrix.m[3][1] - Matrix.m[0][3] * Matrix.m[2][1] * Matrix.m[3][2] + Matrix.m[0][3] * Matrix.m[2][2] * Matrix.m[3][1] + Matrix.m[0][2] * Matrix.m[2][1] * Matrix.m[3][3] + Matrix.m[0][1] * Matrix.m[2][3] * Matrix.m[3][2]) / determinant;
	returnMatrix.m[0][2] =  (Matrix.m[0][1] * Matrix.m[1][2] * Matrix.m[3][3] + Matrix.m[0][2] * Matrix.m[1][3] * Matrix.m[3][1] + Matrix.m[0][3] * Matrix.m[1][1] * Matrix.m[3][2] - Matrix.m[0][3] * Matrix.m[1][2] * Matrix.m[3][1] - Matrix.m[0][2] * Matrix.m[1][1] * Matrix.m[3][3] - Matrix.m[0][1] * Matrix.m[1][3] * Matrix.m[3][2]) / determinant;
	returnMatrix.m[0][3] = (-Matrix.m[0][1] * Matrix.m[1][2] * Matrix.m[2][3] - Matrix.m[0][2] * Matrix.m[1][3] * Matrix.m[2][1] - Matrix.m[0][3] * Matrix.m[1][1] * Matrix.m[2][2] + Matrix.m[0][3] * Matrix.m[1][2] * Matrix.m[2][1] + Matrix.m[0][2] * Matrix.m[1][1] * Matrix.m[2][3] + Matrix.m[0][1] * Matrix.m[1][3] * Matrix.m[2][2]) / determinant;
	
	returnMatrix.m[1][0] = (-Matrix.m[1][0] * Matrix.m[2][2] * Matrix.m[3][3] - Matrix.m[1][2] * Matrix.m[2][3] * Matrix.m[3][0] - Matrix.m[1][3] * Matrix.m[2][0] * Matrix.m[3][2] + Matrix.m[1][3] * Matrix.m[2][2] * Matrix.m[3][0] + Matrix.m[1][2] * Matrix.m[2][0] * Matrix.m[3][3] + Matrix.m[1][0] * Matrix.m[2][3] * Matrix.m[3][2]) / determinant;
	returnMatrix.m[1][1] =  (Matrix.m[0][0] * Matrix.m[2][2] * Matrix.m[3][3] + Matrix.m[0][2] * Matrix.m[2][3] * Matrix.m[3][0] + Matrix.m[0][3] * Matrix.m[2][0] * Matrix.m[3][2] - Matrix.m[0][3] * Matrix.m[2][2] * Matrix.m[3][0] - Matrix.m[0][2] * Matrix.m[2][0] * Matrix.m[3][3] - Matrix.m[0][0] * Matrix.m[2][3] * Matrix.m[3][2]) / determinant;
	returnMatrix.m[1][2] = (-Matrix.m[0][0] * Matrix.m[1][2] * Matrix.m[3][3] - Matrix.m[0][2] * Matrix.m[1][3] * Matrix.m[3][0] - Matrix.m[0][3] * Matrix.m[1][0] * Matrix.m[3][2] + Matrix.m[0][3] * Matrix.m[1][2] * Matrix.m[3][0] + Matrix.m[0][2] * Matrix.m[1][0] * Matrix.m[3][3] + Matrix.m[0][0] * Matrix.m[1][3] * Matrix.m[3][2]) / determinant;
	returnMatrix.m[1][3] =  (Matrix.m[0][0] * Matrix.m[1][2] * Matrix.m[2][3] + Matrix.m[0][2] * Matrix.m[1][3] * Matrix.m[2][0] + Matrix.m[0][3] * Matrix.m[1][0] * Matrix.m[2][2] - Matrix.m[0][3] * Matrix.m[1][2] * Matrix.m[2][0] - Matrix.m[0][2] * Matrix.m[1][0] * Matrix.m[2][3] - Matrix.m[0][0] * Matrix.m[1][3] * Matrix.m[2][2]) / determinant;
	
	returnMatrix.m[2][0] =  (Matrix.m[1][0] * Matrix.m[2][1] * Matrix.m[3][3] + Matrix.m[1][1] * Matrix.m[2][3] * Matrix.m[3][0] + Matrix.m[1][3] * Matrix.m[2][0] * Matrix.m[3][1] - Matrix.m[1][3] * Matrix.m[2][1] * Matrix.m[3][0] - Matrix.m[1][1] * Matrix.m[2][0] * Matrix.m[3][3] - Matrix.m[1][0] * Matrix.m[2][3] * Matrix.m[3][1]) / determinant;
	returnMatrix.m[2][1] = (-Matrix.m[0][0] * Matrix.m[2][1] * Matrix.m[3][3] - Matrix.m[0][1] * Matrix.m[2][3] * Matrix.m[3][0] - Matrix.m[0][3] * Matrix.m[2][0] * Matrix.m[3][1] + Matrix.m[0][3] * Matrix.m[2][1] * Matrix.m[3][0] + Matrix.m[0][1] * Matrix.m[2][0] * Matrix.m[3][3] + Matrix.m[0][0] * Matrix.m[2][3] * Matrix.m[3][1]) / determinant;
	returnMatrix.m[2][2] =  (Matrix.m[0][0] * Matrix.m[1][1] * Matrix.m[3][3] + Matrix.m[0][1] * Matrix.m[1][3] * Matrix.m[3][0] + Matrix.m[0][3] * Matrix.m[1][0] * Matrix.m[3][1] - Matrix.m[0][3] * Matrix.m[1][1] * Matrix.m[3][0] - Matrix.m[0][1] * Matrix.m[1][0] * Matrix.m[3][3] - Matrix.m[0][0] * Matrix.m[1][3] * Matrix.m[3][1]) / determinant;
	returnMatrix.m[2][3] = (-Matrix.m[0][0] * Matrix.m[1][1] * Matrix.m[2][3] - Matrix.m[0][1] * Matrix.m[1][3] * Matrix.m[2][0] - Matrix.m[0][3] * Matrix.m[1][0] * Matrix.m[2][1] + Matrix.m[0][3] * Matrix.m[1][1] * Matrix.m[2][0] + Matrix.m[0][1] * Matrix.m[1][0] * Matrix.m[2][3] + Matrix.m[0][0] * Matrix.m[1][3] * Matrix.m[2][1]) / determinant;

	returnMatrix.m[3][0] = (-Matrix.m[1][0] * Matrix.m[2][1] * Matrix.m[3][2] - Matrix.m[1][1] * Matrix.m[2][2] * Matrix.m[3][0] - Matrix.m[1][2] * Matrix.m[2][0] * Matrix.m[3][1] + Matrix.m[1][2] * Matrix.m[2][1] * Matrix.m[3][0] + Matrix.m[1][1] * Matrix.m[2][0] * Matrix.m[3][2] + Matrix.m[1][0] * Matrix.m[2][2] * Matrix.m[3][1]) / determinant;
	returnMatrix.m[3][1] =  (Matrix.m[0][0] * Matrix.m[2][1] * Matrix.m[3][2] + Matrix.m[0][1] * Matrix.m[2][2] * Matrix.m[3][0] + Matrix.m[0][2] * Matrix.m[2][0] * Matrix.m[3][1] - Matrix.m[0][2] * Matrix.m[2][1] * Matrix.m[3][0] - Matrix.m[0][1] * Matrix.m[2][0] * Matrix.m[3][2] - Matrix.m[0][0] * Matrix.m[2][2] * Matrix.m[3][1]) / determinant;
	returnMatrix.m[3][2] = (-Matrix.m[0][0] * Matrix.m[1][1] * Matrix.m[3][2] - Matrix.m[0][1] * Matrix.m[1][2] * Matrix.m[3][0] - Matrix.m[0][2] * Matrix.m[1][0] * Matrix.m[3][1] + Matrix.m[0][2] * Matrix.m[1][1] * Matrix.m[3][0] + Matrix.m[0][1] * Matrix.m[1][0] * Matrix.m[3][2] + Matrix.m[0][0] * Matrix.m[1][2] * Matrix.m[3][1]) / determinant;
	returnMatrix.m[3][3] = (Matrix.m[0][0] * Matrix.m[1][1] * Matrix.m[2][2] + Matrix.m[0][1] * Matrix.m[1][2] * Matrix.m[2][0] + Matrix.m[0][2] * Matrix.m[1][0] * Matrix.m[2][1] - Matrix.m[0][2] * Matrix.m[1][1] * Matrix.m[2][0] - Matrix.m[0][1] * Matrix.m[1][0] * Matrix.m[2][2] - Matrix.m[0][0] * Matrix.m[1][2] * Matrix.m[2][1]) / determinant;
	return returnMatrix;
}

Matrix4x4 MakeScaleMatrix(Vector3 scale) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = scale.x; returnMatrix.m[0][1] = 0.0f; returnMatrix.m[0][2] = 0.0f; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 0.0f; returnMatrix.m[1][1] = scale.y; returnMatrix.m[1][2] = 0.0f; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 0.0f; returnMatrix.m[2][1] = 0.0f; returnMatrix.m[2][2] = scale.z; returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = 0.0f; returnMatrix.m[3][1] = 0.0f; returnMatrix.m[3][2] = 0.0f; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

Matrix4x4 MakeTranslateMatrix(Vector3 translate) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = 1.0f; returnMatrix.m[0][1] = 0.0f; returnMatrix.m[0][2] = 0.0f; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 0.0f; returnMatrix.m[1][1] = 1.0f; returnMatrix.m[1][2] = 0.0f; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 0.0f; returnMatrix.m[2][1] = 0.0f; returnMatrix.m[2][2] = 1.0f; returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = translate.x; returnMatrix.m[3][1] = translate.y; returnMatrix.m[3][2] = translate.z; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

Matrix4x4 MakeXRotateMatrix(float theta) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = 1.0f; returnMatrix.m[0][1] = 0.0f; returnMatrix.m[0][2] = 0.0f; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 0.0f; returnMatrix.m[1][1] = cosf(theta * float(M_PI)); returnMatrix.m[1][2] = sinf(theta * float(M_PI)); returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 0.0f; returnMatrix.m[2][1] = -sinf(theta * float(M_PI)); returnMatrix.m[2][2] = cosf(theta * float(M_PI)); returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = 0.0f; returnMatrix.m[3][1] = 0.0f; returnMatrix.m[3][2] = 0.0f; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

Matrix4x4 MakeYRotateMatrix(float theta) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = cosf(theta * float(M_PI)); returnMatrix.m[0][1] = 0.0f; returnMatrix.m[0][2] = -sinf(theta * float(M_PI)); returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 0.0f; returnMatrix.m[1][1] = 1.0f; returnMatrix.m[1][2] = 0.0f; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = sinf(theta * float(M_PI)); returnMatrix.m[2][1] = 0.0f; returnMatrix.m[2][2] = cosf(theta * float(M_PI)); returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = 0.0f; returnMatrix.m[3][1] = 0.0f; returnMatrix.m[3][2] = 0.0f; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

Matrix4x4 MakeZRotateMatrix(float theta) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = cosf(theta * float(M_PI)); returnMatrix.m[0][1] = sinf(theta * float(M_PI)); returnMatrix.m[0][2] = 0.0f; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = -sinf(theta * float(M_PI)); returnMatrix.m[1][1] = cosf(theta * float(M_PI)); returnMatrix.m[1][2] = 0.0f; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 0.0f; returnMatrix.m[2][1] = 0.0f; returnMatrix.m[2][2] = 1.0f; returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = 0.0f; returnMatrix.m[3][1] = 0.0f; returnMatrix.m[3][2] = 0.0f; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotate, Vector3 translate) {
	Matrix4x4 rotateX = MakeXRotateMatrix(rotate.x);
	Matrix4x4 rotateY = MakeYRotateMatrix(rotate.y);
	Matrix4x4 rotateZ = MakeZRotateMatrix(rotate.z);

	Matrix4x4 rotateMatrix = Multipty(rotateX, Multipty(rotateY, rotateZ));

	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = scale.x * rotateMatrix.m[0][0]; returnMatrix.m[0][1] = scale.x * rotateMatrix.m[0][1]; returnMatrix.m[0][2] = scale.x * rotateMatrix.m[0][2]; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = scale.y * rotateMatrix.m[1][0]; returnMatrix.m[1][1] = scale.y * rotateMatrix.m[1][1]; returnMatrix.m[1][2] = scale.y * rotateMatrix.m[1][2]; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = scale.z * rotateMatrix.m[2][0]; returnMatrix.m[2][1] = scale.z * rotateMatrix.m[2][1]; returnMatrix.m[2][2] = scale.z * rotateMatrix.m[2][2]; returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = translate.x; returnMatrix.m[3][1] = translate.y; returnMatrix.m[3][2] = translate.z; returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = 1 / tanf(fovY / 2) / aspectRatio; returnMatrix.m[0][1] = 0.0f; returnMatrix.m[0][2] = 0.0f; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 0.0f; returnMatrix.m[1][1] = 1 / tanf(fovY / 2); returnMatrix.m[1][2] = 0.0f; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 0.0f; returnMatrix.m[2][1] = 0.0f; returnMatrix.m[2][2] = farClip / (farClip - nearClip); returnMatrix.m[2][3] = 1.0f;
	returnMatrix.m[3][0] = 0.0f; returnMatrix.m[3][1] = 0.0f; returnMatrix.m[3][2] = -nearClip * farClip / (farClip - nearClip); returnMatrix.m[3][3] = 0.0f;
	return returnMatrix;
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = 2 / (right - left); returnMatrix.m[0][1] = 0.0f; returnMatrix.m[0][2] = 0.0f; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = 0.0f; returnMatrix.m[1][1] = 2 / (top - bottom); returnMatrix.m[1][2] = 0.0f; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = 0.0f; returnMatrix.m[2][1] = 0.0f; returnMatrix.m[2][2] = 1 / (farClip - nearClip); returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = (left + right) / (left - right); returnMatrix.m[3][1] = (top + bottom) / (bottom - top); returnMatrix.m[3][2] = nearClip / (nearClip - farClip); returnMatrix.m[3][3] = 1.0f;
	return returnMatrix;
}