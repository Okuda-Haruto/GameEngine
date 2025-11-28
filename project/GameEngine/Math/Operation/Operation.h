#pragma once
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix3x3.h>
#include <Matrix4x4.h>
#include <vector>

#pragma region Vector2

//加算
Vector2 Add(const Vector2& v1, const Vector2& v2);
//減算
Vector2 Subtract(const Vector2& v1, const Vector2& v2);
//スカラー倍
Vector2 Multiply(float scalar, const Vector2& v);
//内積
float Dot(const Vector2& v1, const Vector2& v2);
//長さ(ノルム)
float Length(const Vector2& v);
//正規化
Vector2 Normalize(const Vector2& v);
// 線形補間
Vector2 Lerp(Vector2 a, Vector2 b, float t);

Vector2 operator+(const Vector2& v1, const Vector2& v2);
Vector2 operator-(const Vector2& v1, const Vector2& v2);
Vector2 operator*(float s, const Vector2& v);
Vector2 operator*(const Vector2& v, float s);
Vector2 operator/(const Vector2& v, float s);
Vector2 operator-(const Vector2& v);
Vector2 operator+(const Vector2& v);

#pragma endregion

#pragma region Vector3

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

//変換正規化
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);
// 線形補間
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);
Vector3 Bezier(const Vector3& v0, const Vector3& v1, const Vector3& v2, float t);
Vector3 Spline(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3, float t);
Vector3 Spline(const std::vector<Vector3>& controlPoint, float t);

Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator-(const Vector3& v1, const Vector3& v2);
Vector3 operator*(float s, const Vector3& v);
Vector3 operator*(const Vector3& v, float s);
Vector3 operator/(const Vector3& v, float s);
Vector3 operator-(const Vector3& v);
Vector3 operator+(const Vector3& v);

#pragma endregion

#pragma region Vector4

//加算
Vector4 Add(const Vector4& v1, const Vector4& v2);
//減算
Vector4 Subtract(const Vector4& v1, const Vector4& v2);
//スカラー倍
Vector4 Multiply(float scalar, const Vector4& v);
//内積
float Dot(const Vector4& v1, const Vector4& v2);
//長さ(ノルム)
float Length(const Vector4& v);
//正規化
Vector4 Normalize(const Vector4& v);
// 線形補間
Vector4 Lerp(Vector4 a, Vector4 b, float t);

Vector4 operator+(const Vector4& v1, const Vector4& v2);
Vector4 operator-(const Vector4& v1, const Vector4& v2);
Vector4 operator*(float s, const Vector4& v);
Vector4 operator*(const Vector4& v, float s);
Vector4 operator/(const Vector4& v, float s);
Vector4 operator-(const Vector4& v);
Vector4 operator+(const Vector4& v);

#pragma endregion

#pragma region Matrix4x4

//行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);
//行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);
//行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
//逆行列
Matrix4x4 Inverse(const Matrix4x4& m);
//転置行列
Matrix4x4 Transpose(const Matrix4x4& m);
//単位表列の作成
Matrix4x4 MakeIdentity4x4();

//平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
//拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
//座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

//X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);
//Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);
//Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);
//回転行列
Matrix4x4 MakeRotateMatrix(Vector3 rotate);

//3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotate, Vector3 translate);

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
//正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);
Vector3 operator*(const Vector3& v, const Matrix4x4& m);
Vector3 operator*(const Matrix4x4& m, const Vector3& v);

#pragma endregion