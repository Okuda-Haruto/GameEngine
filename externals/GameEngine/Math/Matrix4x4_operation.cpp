#include "Matrix4x4_operation.h"
#include <assert.h>
#include <cmath>

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

//3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotate, Vector3 translate) {
	Matrix4x4 rotateX = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateY = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZ = MakeRotateZMatrix(rotate.z);

	Matrix4x4 rotateMatrix = Multiply(rotateX, Multiply(rotateY, rotateZ));

	Matrix4x4 returnMatrix;
	returnMatrix.m[0][0] = scale.x * rotateMatrix.m[0][0]; returnMatrix.m[0][1] = scale.x * rotateMatrix.m[0][1]; returnMatrix.m[0][2] = scale.x * rotateMatrix.m[0][2]; returnMatrix.m[0][3] = 0.0f;
	returnMatrix.m[1][0] = scale.y * rotateMatrix.m[1][0]; returnMatrix.m[1][1] = scale.y * rotateMatrix.m[1][1]; returnMatrix.m[1][2] = scale.y * rotateMatrix.m[1][2]; returnMatrix.m[1][3] = 0.0f;
	returnMatrix.m[2][0] = scale.z * rotateMatrix.m[2][0]; returnMatrix.m[2][1] = scale.z * rotateMatrix.m[2][1]; returnMatrix.m[2][2] = scale.z * rotateMatrix.m[2][2]; returnMatrix.m[2][3] = 0.0f;
	returnMatrix.m[3][0] = translate.x; returnMatrix.m[3][1] = translate.y; returnMatrix.m[3][2] = translate.z; returnMatrix.m[3][3] = 1.0f;
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