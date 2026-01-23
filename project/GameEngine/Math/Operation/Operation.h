#pragma once
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Quaternion.h>
#include <Matrix3x3.h>
#include <Matrix4x4.h>
#include <assimp/matrix4x4.h>
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
Vector4 NormalizeWeights(const Vector4& v);
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

#pragma region Quaternion

//加算
Quaternion Add(const Quaternion& q0, const Quaternion& q1);

//減算
Quaternion Subtract(const Quaternion& q0, const Quaternion& q1);

//スカラー倍
Quaternion Multiply(const float& f, const Quaternion& q);

//乗算
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

//単位クオータニオン
Quaternion IdentityQuaternion();

Quaternion Conjugate(const Quaternion& quaternion);

float Norm(const Quaternion& quaternion);

Quaternion Normalize(const Quaternion& quaternion);

Quaternion Inverse(const Quaternion& quaternion);

Quaternion Log(const Quaternion& quaternion);

//任意軸回転を表すQuaternionの生成
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);
//ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

//内積
float Dot(const Quaternion& q0, const Quaternion& q1);

//球面線形補間
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

//球面スプライン
Quaternion Squad(const Quaternion& q0, const Quaternion& q1, const Quaternion& q2, const Quaternion& q3, float t);

Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
Quaternion operator-(const Quaternion& q1, const Quaternion& q2);
Quaternion operator*(float s, const Quaternion& q);
Quaternion operator*(const Quaternion& q, float s);
Quaternion operator*(const Quaternion& q1, Quaternion q2);
Quaternion operator/(const Quaternion& q, float s);
Quaternion operator-(const Quaternion& q);
Quaternion operator+(const Quaternion& q);

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
//任意軸回転行列
Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);
//Quaternionからの回転行列
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);

//3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotate, Vector3 translate);
//クォータニオン行列
Matrix4x4 MakeQuaternionMatrix(Vector3 scale, Quaternion rotate, Vector3 translate);

//内積行列
Matrix4x4 DotMatrix(const Vector3& v1, const Vector3& v2);
//クロス積行列
Matrix4x4 CrossMatrix(const Vector3& v);

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
//正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

//aiMatrix変換(GLTF用)
Matrix4x4 aiMatrix4x4ToMatrix4x4(aiMatrix4x4 matrix);

Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator*(const Matrix4x4& m, const float f);
Matrix4x4 operator*(const float f, const Matrix4x4& m);
Vector3 operator*(const Vector3& v, const Matrix4x4& m);
Vector3 operator*(const Matrix4x4& m, const Vector3& v);

#pragma endregion