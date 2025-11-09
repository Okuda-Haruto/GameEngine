#pragma once

struct Matrix4x4;

struct Vector3 {
	float x;
	float y;
	float z;

	Vector3& operator*=(float s) { x *= s; y *= s; z *= s;	return *this; }
	Vector3& operator-=(const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vector3& operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
	Vector3& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }

	//
	//	Vector3::で呼び出す
	//

	//加算
	static Vector3 Add(const Vector3& v1, const Vector3& v2);
	//減算
	static Vector3 Subtract(const Vector3& v1, const Vector3& v2);
	//スカラー倍
	static Vector3 Multiply(float scalar, const Vector3& v);
	//内積
	static float Dot(const Vector3& v1, const Vector3& v2);
	//長さ(ノルム)
	static float Length(const Vector3& v);
	//正規化
	static Vector3 Normalize(const Vector3& v);

	//クロス積
	static Vector3 Cross(const Vector3& v1, const Vector3& v2);
	static Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);
};

Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator-(const Vector3& v1, const Vector3& v2);
Vector3 operator*(float s, const Vector3& v);
Vector3 operator*(const Vector3& v, float s);
Vector3 operator/(const Vector3& v, float s);
Vector3 operator-(const Vector3& v);
Vector3 operator+(const Vector3& v);