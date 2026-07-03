#define NOMINMAX
#include "Collision.h"
#include "Vector3.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Operation/Operation.h"
#include <cmath>
#include <algorithm>

//正射影ベクトル
Vector3 Project(const Vector3& v1, const Vector3& v2) {
	float t;
	Vector3 vector;
	t = Dot(v1,v2) / powf(Length(v2),2);
	vector = t * v2;
	return vector;
}

//最終接点
Vector3 ClosestPoint(const Vector3& point, const Line& line) {
	float t = Dot(point - line.origin, line.diff)
		/ Dot(line.diff, line.diff);

	return line.origin + line.diff * t;
}
Vector3 ClosestPoint(const Vector3& point, const Ray& ray) {
	float t = Dot(point - ray.origin, ray.diff)
		/ Dot(ray.diff, ray.diff);

	// Rayなので0以上に制限
	t = std::max(t, 0.0f);

	return ray.origin + ray.diff * t;
}
Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	float t = Dot(point - segment.origin, segment.diff)
		/ Dot(segment.diff, segment.diff);

	// 線分なので0～1に制限
	t = std::clamp(t, 0.0f, 1.0f);

	return segment.origin + segment.diff * t;
}

//球と球の衝突
bool IsCollision(const Sphere& s1, const Sphere& s2) {
	float distance = Length(s2.center - s1.center);

	return distance <= s1.radius + s2.radius;
}

//球と平面の衝突
bool IsCollision(const Sphere& sphere, const Plane& plane) {
	float k = Dot(plane.normal, sphere.center) - plane.distance;

	return fabsf(k) <= sphere.radius;
}

//直線と平面の衝突
bool IsCollision(const Line& line, const Plane& plane) {
	if ((plane.distance - Dot(line.origin, plane.normal)) == 0.0f) {
		return false;
	}

	return true;
}

//半直線と平面の衝突
bool IsCollision(const Ray& ray, const Plane& plane) {
	if ((plane.distance - Dot(ray.origin, plane.normal)) == 0.0f) {
		return false;
	}

	float t = (plane.distance - Dot(ray.origin, plane.normal)) / Dot(ray.diff, plane.normal);

	if (t > 0.0f) {
		return true;
	} else {
		return false;
	}
}

//線分と平面の衝突
bool IsCollision(const Segment& segment, const Plane& plane) {
	if ((plane.distance - Dot(segment.origin, plane.normal)) == 0.0f) {
		return false;
	}

	float t = (plane.distance - Dot(segment.origin, plane.normal)) / Dot(segment.diff, plane.normal);

	if (t > 0.0f && t <= 1.0f) {
		return true;
	} else {
		return false;
	}
}

//直線と球の当たり判定
bool IsCollision(const Line& line, const Sphere& sphere){
	Vector3 closest = ClosestPoint(sphere.center, line);

	float distance = Length(closest - sphere.center);

	return distance <= sphere.radius;
}

//半直線と球の当たり判定
bool IsCollision(const Ray& ray, const Sphere& sphere){
	Vector3 closest = ClosestPoint(sphere.center, ray);

	float distance = Length(closest - sphere.center);

	return distance <= sphere.radius;
}

//線分と球の当たり判定
bool IsCollision(const Segment& segment, const Sphere& sphere){
	Vector3 closest = ClosestPoint(sphere.center, segment);

	float distance = Length(closest - sphere.center);

	return distance <= sphere.radius;
}

//三角形と線分の衝突
bool IsCollision(const Triangle& triangle, const Segment& segment) {
	Vector3 v01 = triangle.vertices[0] - triangle.vertices[1];
	Vector3 v12 = triangle.vertices[1] - triangle.vertices[2];
	Vector3 v20 = triangle.vertices[2] - triangle.vertices[0];
	Plane plane;
	plane.normal = Normalize(Cross(v01, v12));
	plane.distance = Dot(triangle.vertices[0], plane.normal);

	if (IsCollision(segment, plane)) {
		float t = (plane.distance - Dot(segment.origin, plane.normal)) / Dot(segment.diff, plane.normal);
		Vector3 p = segment.origin + t * segment.diff;
		Vector3 v0p = triangle.vertices[0] - p;
		Vector3 v1p = triangle.vertices[1] - p;
		Vector3 v2p = triangle.vertices[2] - p;

		Vector3 cross01 = Cross(v01, v1p);
		Vector3 cross12 = Cross(v12, v2p);
		Vector3 cross20 = Cross(v20, v0p);

		if (Dot(cross01, plane.normal) >= 0.0f &&
			Dot(cross12, plane.normal) >= 0.0f &&
			Dot(cross20, plane.normal) >= 0.0f) {
			return true;
		}
	}

	return false;
}

//AABBと点の衝突
bool IsCollision(const AABB& aabb, const Vector3& point) {
	if (aabb.min.x <= point.x && aabb.max.x >= point.x &&
		aabb.min.y <= point.y && aabb.max.y >= point.y &&
		aabb.min.z <= point.z && aabb.max.z >= point.z) {
		return true;
	}
	return false;
}

//AABBとAABBの衝突
bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
	if ((aabb1.min.x <= aabb2.max.x && aabb2.min.x <= aabb1.max.x) &&
		(aabb1.min.y <= aabb2.max.y && aabb2.min.y <= aabb1.max.y) &&
		(aabb1.min.z <= aabb2.max.z && aabb2.min.z <= aabb1.max.z)) {
		return true;
	}
	return false;
}

//AABBと球の衝突
bool IsCollision(const AABB& aabb, const Sphere& sphere) {
	Vector3 clossPoint{
		std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),
		std::clamp(sphere.center.y,aabb.min.y,aabb.max.y),
		std::clamp(sphere.center.z,aabb.min.z,aabb.max.z)
	};

	//最近接点と球の中心との距離を求める
	float distance = Length(clossPoint - sphere.center);

	//距離が半径よりも小さければ衝突
	if (distance <= sphere.radius) {
		return true;
	}
	return false;
}

//AABBと直線の衝突
bool IsCollision(const AABB& aabb, const Line& line) {

	Vector3 min = {
		(aabb.min.x - line.origin.x) / line.diff.x,
		(aabb.min.y - line.origin.y) / line.diff.y,
		(aabb.min.z - line.origin.z) / line.diff.z,
	};
	Vector3 max = {
		(aabb.max.x - line.origin.x) / line.diff.x,
		(aabb.max.y - line.origin.y) / line.diff.y,
		(aabb.max.z - line.origin.z) / line.diff.z,
	};

	float tNearX = std::min(min.x, max.x), tFarX = std::max(min.x, max.x);
	float tNearY = std::min(min.y, max.y), tFarY = std::max(min.y, max.y);
	float tNearZ = std::min(min.z, max.z), tFarZ = std::max(min.z, max.z);

	//AABBとの衝突点(貫通点)のtが小さい方
	float tmin = std::max(std::max(tNearX, tNearY), tNearZ);
	//AABBとの衝突点(貫通点)のtが大きい方
	float tmax = std::min(std::min(tFarX, tFarY), tFarZ);

	if (tmin <= tmax) {
		return true;
	}
	return false;
}
//AABBと半直線の衝突
bool IsCollision(const AABB& aabb, const Ray& ray) {

	Vector3 min = {
		(aabb.min.x - ray.origin.x) / ray.diff.x,
		(aabb.min.y - ray.origin.y) / ray.diff.y,
		(aabb.min.z - ray.origin.z) / ray.diff.z,
	};
	Vector3 max = {
		(aabb.max.x - ray.origin.x) / ray.diff.x,
		(aabb.max.y - ray.origin.y) / ray.diff.y,
		(aabb.max.z - ray.origin.z) / ray.diff.z,
	};

	float tNearX = std::min(min.x, max.x), tFarX = std::max(min.x, max.x);
	float tNearY = std::min(min.y, max.y), tFarY = std::max(min.y, max.y);
	float tNearZ = std::min(min.z, max.z), tFarZ = std::max(min.z, max.z);

	//AABBとの衝突点(貫通点)のtが小さい方
	float tmin = std::max(std::max(tNearX, tNearY), tNearZ);
	//AABBとの衝突点(貫通点)のtが大きい方
	float tmax = std::min(std::min(tFarX, tFarY), tFarZ);

	if (tmin <= tmax && tmax >= 0.0f) {
		return true;
	}
	return false;
}

//AABBと線分の衝突
bool IsCollision(const AABB& aabb, const Segment& segment) {

	Vector3 min = {
		(aabb.min.x - segment.origin.x) / segment.diff.x,
		(aabb.min.y - segment.origin.y) / segment.diff.y,
		(aabb.min.z - segment.origin.z) / segment.diff.z,
	};
	//Nan対策
	if ((aabb.min.x - segment.origin.x) == 0 && segment.diff.x == 0) {
		min.x = (aabb.min.x - (segment.origin.x - 0.00001f)) / (segment.diff.x - 0.00001f);
	}
	if ((aabb.min.y - segment.origin.y) == 0 && segment.diff.y == 0) {
		min.y = (aabb.min.y - (segment.origin.y - 0.00001f)) / (segment.diff.y - 0.00001f);
	}
	if ((aabb.min.z - segment.origin.z) == 0 && segment.diff.z == 0) {
		min.z = (aabb.min.z - (segment.origin.z - 0.00001f)) / (segment.diff.z - 0.00001f);
	}

	Vector3 max = {
		(aabb.max.x - segment.origin.x) / segment.diff.x,
		(aabb.max.y - segment.origin.y) / segment.diff.y,
		(aabb.max.z - segment.origin.z) / segment.diff.z,
	};
	//Nan対策
	if ((aabb.max.x - segment.origin.x) == 0 && segment.diff.x == 0) {
		max.x = (aabb.max.x - (segment.origin.x - 0.00001f)) / (segment.diff.x - 0.00001f);
	}
	if ((aabb.max.y - segment.origin.y) == 0 && segment.diff.y == 0) {
		max.y = (aabb.max.y - (segment.origin.y - 0.00001f)) / (segment.diff.y - 0.00001f);
	}
	if ((aabb.max.z - segment.origin.z) == 0 && segment.diff.z == 0) {
		max.z = (aabb.max.z - (segment.origin.z - 0.00001f)) / (segment.diff.z - 0.00001f);
	}

	float tNearX = std::min(min.x, max.x), tFarX = std::max(min.x, max.x);
	float tNearY = std::min(min.y, max.y), tFarY = std::max(min.y, max.y);
	float tNearZ = std::min(min.z, max.z), tFarZ = std::max(min.z, max.z);

	//AABBとの衝突点(貫通点)のtが小さい方
	float tmin = std::max(std::max(tNearX, tNearY), tNearZ);
	//AABBとの衝突点(貫通点)のtが大きい方
	float tmax = std::min(std::min(tFarX, tFarY), tFarZ);

	if (fabsf(tmin) == INFINITY || fabsf(tmax) == INFINITY) {
		return true;
	}

	if (tmin <= tmax && tmin <= 1.0f && tmax >= 0.0f) {
		return true;
	}
	return false;
}

//OBBと球の衝突
bool IsCollision(const OBB& obb, const Sphere& sphere) {

	Matrix4x4 obbWorldMatrix{
		.m{
			{obb.orientations[0].x	,obb.orientations[0].y	,obb.orientations[0].z	,0.0f},
			{obb.orientations[1].x	,obb.orientations[1].y	,obb.orientations[1].z	,0.0f},
			{obb.orientations[2].x	,obb.orientations[2].y	,obb.orientations[2].z	,0.0f},
			{obb.center.x			,obb.center.y			,obb.center.z			,1.0f},
		}
	};

	Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);

	Vector3 centerInOBBLocalSpace = sphere.center * obbWorldMatrixInverse;

	AABB aabbOBBLocal{
		.min{-obb.size.x,-obb.size.y,-obb.size.z},
		.max{obb.size}
	};

	Sphere sphereOBBLocal{
		.center{centerInOBBLocalSpace},
		.radius{sphere.radius}
	};

	return IsCollision(aabbOBBLocal, sphereOBBLocal);
}

//OBBと直線の衝突
bool IsCollision(const OBB& obb, const Line& line) {

	Matrix4x4 obbWorldMatrix{
		.m{
			{obb.orientations[0].x	,obb.orientations[0].y	,obb.orientations[0].z	,0.0f},
			{obb.orientations[1].x	,obb.orientations[1].y	,obb.orientations[1].z	,0.0f},
			{obb.orientations[2].x	,obb.orientations[2].y	,obb.orientations[2].z	,0.0f},
			{obb.center.x			,obb.center.y			,obb.center.z			,1.0f},
		}
	};

	Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);

	Vector3 localOrigin = line.origin * obbWorldMatrixInverse;
	Vector3 localEnd = (line.origin + line.diff) * obbWorldMatrixInverse;

	AABB localAABB{
		{-obb.size.x,-obb.size.y,-obb.size.z},
		{+obb.size.x,+obb.size.y,+obb.size.z}
	};

	Line localLine;
	localLine.origin = localOrigin;
	localLine.diff = localEnd - localOrigin;

	return IsCollision(localAABB, localLine);
}

//OBBと半直線の衝突
bool IsCollision(const OBB& obb, const Ray& ray) {

	Matrix4x4 obbWorldMatrix{
		.m{
			{obb.orientations[0].x	,obb.orientations[0].y	,obb.orientations[0].z	,0.0f},
			{obb.orientations[1].x	,obb.orientations[1].y	,obb.orientations[1].z	,0.0f},
			{obb.orientations[2].x	,obb.orientations[2].y	,obb.orientations[2].z	,0.0f},
			{obb.center.x			,obb.center.y			,obb.center.z			,1.0f},
		}
	};

	Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);

	Vector3 localOrigin = ray.origin * obbWorldMatrixInverse;
	Vector3 localEnd = (ray.origin + ray.diff) * obbWorldMatrixInverse;

	AABB localAABB{
		{-obb.size.x,-obb.size.y,-obb.size.z},
		{+obb.size.x,+obb.size.y,+obb.size.z}
	};

	Ray localRay;
	localRay.origin = localOrigin;
	localRay.diff = localEnd - localOrigin;

	return IsCollision(localAABB, localRay);
}

//OBBと線分の衝突
bool IsCollision(const OBB& obb, const Segment& segment) {

	Matrix4x4 obbWorldMatrix{
		.m{
			{obb.orientations[0].x	,obb.orientations[0].y	,obb.orientations[0].z	,0.0f},
			{obb.orientations[1].x	,obb.orientations[1].y	,obb.orientations[1].z	,0.0f},
			{obb.orientations[2].x	,obb.orientations[2].y	,obb.orientations[2].z	,0.0f},
			{obb.center.x			,obb.center.y			,obb.center.z			,1.0f},
		}
	};

	Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);

	Vector3 localOrigin = segment.origin * obbWorldMatrixInverse;
	Vector3 localEnd = (segment.origin + segment.diff) * obbWorldMatrixInverse;

	AABB localAABB{
		{-obb.size.x,-obb.size.y,-obb.size.z},
		{+obb.size.x,+obb.size.y,+obb.size.z}
	};

	Segment localSegment;
	localSegment.origin = localOrigin;
	localSegment.diff = localEnd + localOrigin;

	return IsCollision(localAABB, localSegment);
}

//OBBとOBBの衝突
bool IsCollision(const OBB& obb1, const OBB& obb2) {

	constexpr float EPSILON = 1e-6f;

	// OBBの軸
	Vector3 A[3] = {
		Normalize(obb1.orientations[0]),
		Normalize(obb1.orientations[1]),
		Normalize(obb1.orientations[2])
	};
	Vector3 B[3] = {
		Normalize(obb2.orientations[0]),
		Normalize(obb2.orientations[1]),
		Normalize(obb2.orientations[2])
	};

	// 回転行列
	Matrix3x3 R;
	Matrix3x3 AbsR;

	//各軸の向きの近さ
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			R.m[i][j] = Dot(A[i], B[j]);
			AbsR.m[i][j] = std::abs(R.m[i][j]) + EPSILON;
		}
	}

	//OBB1からOBB2へのベクトル
	Vector3 tWorld = obb1.center - obb2.center;

	//OBB1の角度基準でOBB1からOBB2への距離
	Vector3 t{
		Dot(tWorld, A[0]),
		Dot(tWorld, A[1]),
		Dot(tWorld, A[2])
	};

	float ra, rb;

	//-----------------------------
	// Aの軸 (A0,A1,A2)
	//-----------------------------
	for (int i = 0; i < 3; i++) {

		ra = (&obb1.size.x)[i];

		rb =
			obb2.size.x * AbsR.m[i][0] +
			obb2.size.y * AbsR.m[i][1] +
			obb2.size.z * AbsR.m[i][2];

		if (std::abs((&t.x)[i]) > ra + rb) {
			return false;
		}
	}

	//-----------------------------
	// Bの軸 (B0,B1,B2)
	//-----------------------------
	for (int j = 0; j < 3; j++) {

		ra =
			obb1.size.x * AbsR.m[0][j] +
			obb1.size.y * AbsR.m[1][j] +
			obb1.size.z * AbsR.m[2][j];

		rb = (&obb2.size.x)[j];

		float dist =
			std::abs(
				t.x * R.m[0][j] +
				t.y * R.m[1][j] +
				t.z * R.m[2][j]
			);

		if (dist > ra + rb) {
			return false;
		}
	}

	//-----------------------------
	// A0 × B0
	//-----------------------------
	ra = obb1.size.y * AbsR.m[2][0] + obb1.size.z * AbsR.m[1][0];
	rb = obb2.size.y * AbsR.m[0][2] + obb2.size.z * AbsR.m[0][1];
	if (std::abs(t.z * R.m[1][0] - t.y * R.m[2][0]) > ra + rb) return false;

	//-----------------------------
	// A0 × B1
	//-----------------------------
	ra = obb1.size.y * AbsR.m[2][1] + obb1.size.z * AbsR.m[1][1];
	rb = obb2.size.x * AbsR.m[0][2] + obb2.size.z * AbsR.m[0][0];
	if (std::abs(t.z * R.m[1][1] - t.y * R.m[2][1]) > ra + rb) return false;

	//-----------------------------
	// A0 × B2
	//-----------------------------
	ra = obb1.size.y * AbsR.m[2][2] + obb1.size.z * AbsR.m[1][2];
	rb = obb2.size.x * AbsR.m[0][1] + obb2.size.y * AbsR.m[0][0];
	if (std::abs(t.z * R.m[1][2] - t.y * R.m[2][2]) > ra + rb) return false;

	//-----------------------------
	// A1 × B0
	//-----------------------------
	ra = obb1.size.x * AbsR.m[2][0] + obb1.size.z * AbsR.m[0][0];
	rb = obb2.size.y * AbsR.m[1][2] + obb2.size.z * AbsR.m[1][1];
	if (std::abs(t.x * R.m[2][0] - t.z * R.m[0][0]) > ra + rb) return false;

	//-----------------------------
	// A1 × B1
	//-----------------------------
	ra = obb1.size.x * AbsR.m[2][1] + obb1.size.z * AbsR.m[0][1];
	rb = obb2.size.x * AbsR.m[1][2] + obb2.size.z * AbsR.m[1][0];
	if (std::abs(t.x * R.m[2][1] - t.z * R.m[0][1]) > ra + rb) return false;

	//-----------------------------
	// A1 × B2
	//-----------------------------
	ra = obb1.size.x * AbsR.m[2][2] + obb1.size.z * AbsR.m[0][2];
	rb = obb2.size.x * AbsR.m[1][1] + obb2.size.y * AbsR.m[1][0];
	if (std::abs(t.x * R.m[2][2] - t.z * R.m[0][2]) > ra + rb) return false;

	//-----------------------------
	// A2 × B0
	//-----------------------------
	ra = obb1.size.x * AbsR.m[1][0] + obb1.size.y * AbsR.m[0][0];
	rb = obb2.size.y * AbsR.m[2][2] + obb2.size.z * AbsR.m[2][1];
	if (std::abs(t.y * R.m[0][0] - t.x * R.m[1][0]) > ra + rb) return false;

	//-----------------------------
	// A2 × B1
	//-----------------------------
	ra = obb1.size.x * AbsR.m[1][1] + obb1.size.y * AbsR.m[0][1];
	rb = obb2.size.x * AbsR.m[2][2] + obb2.size.z * AbsR.m[2][0];
	if (std::abs(t.y * R.m[0][1] - t.x * R.m[1][1]) > ra + rb) return false;

	//-----------------------------
	// A2 × B2
	//-----------------------------
	ra = obb1.size.x * AbsR.m[1][2] + obb1.size.y * AbsR.m[0][2];
	rb = obb2.size.x * AbsR.m[2][1] + obb2.size.y * AbsR.m[2][0];
	if (std::abs(t.y * R.m[0][2] - t.x * R.m[1][2]) > ra + rb) return false;

	return true;
}

//objモデルと線分の衝突
bool IsCollision(const ModelData& model, const Segment& segment) {

	// ポリゴンの数
	uint32_t modelSize = uint32_t(model.vertices.size()) / 3;

	// ポリゴンの数だけ判定を行う
	for (uint32_t i = 0; i < modelSize;i++) {

		//頂点を3角形に変換
		Triangle triangle;
		for (uint8_t num = 0; num <= 2; num++) {
			triangle.vertices[num] = {
				model.vertices[i * 3 + num].position.x,
				model.vertices[i * 3 + num].position.y,
				model.vertices[i * 3 + num].position.z,
			};
		}

		//三角形と線分の衝突判定
		if (IsCollision(triangle, segment)) { return true; }
	}

	return false;
}