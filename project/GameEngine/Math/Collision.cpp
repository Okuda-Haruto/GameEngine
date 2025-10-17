#define NOMINMAX
#include "Collision.h"
#include "Vector3_operation.h"
#include "Matrix4x4_operation.h"
#include <cmath>
#include <algorithm>

//正射影ベクトル
Vector3 Project(const Vector3& v1, const Vector3& v2) {
	float t;
	Vector3 vector;
	t = Dot(v1,v2) / powf(Length(v2),2);
	vector = Multiply(t, v2);
	return vector;
}

//最終接点
Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 cp;
	Vector3 a = Subtract(point, segment.origin);
	cp = Add(segment.origin, Project(a, segment.diff));
	return cp;
}

//球と球の衝突
bool IsCollision(const Sphere& s1, const Sphere& s2) {
	float distance = Length(Subtract(s2.center,s1.center));

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

//三角形と線分の衝突
bool IsCollision(const Triangle& triangle, const Segment& segment) {
	Vector3 v01 = Subtract(triangle.vertices[0], triangle.vertices[1]);
	Vector3 v12 = Subtract(triangle.vertices[1], triangle.vertices[2]);
	Vector3 v20 = Subtract(triangle.vertices[2], triangle.vertices[0]);
	Plane plane;
	plane.normal = Normalize(Cross(v01, v12));
	plane.distance = Dot(triangle.vertices[0], plane.normal);

	if (IsCollision(segment, plane)) {
		float t = (plane.distance - Dot(segment.origin, plane.normal)) / Dot(segment.diff, plane.normal);
		Vector3 p = Add(segment.origin, Multiply(t, segment.diff));
		Vector3 v0p = Subtract(triangle.vertices[0], p);
		Vector3 v1p = Subtract(triangle.vertices[1], p);
		Vector3 v2p = Subtract(triangle.vertices[2], p);

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
	float distance = Length(Subtract(clossPoint, sphere.center));

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

	Vector3 centerInOBBLocalSpace = Transform(sphere.center, obbWorldMatrixInverse);

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

	Vector3 localOrigin = Transform(line.origin, obbWorldMatrixInverse);
	Vector3 localEnd = Transform(Add(line.origin, line.diff), obbWorldMatrixInverse);

	AABB localAABB{
		{-obb.size.x,-obb.size.y,-obb.size.z},
		{+obb.size.x,+obb.size.y,+obb.size.z}
	};

	Line localLine;
	localLine.origin = localOrigin;
	localLine.diff = Subtract(localEnd, localOrigin);

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

	Vector3 localOrigin = Transform(ray.origin, obbWorldMatrixInverse);
	Vector3 localEnd = Transform(Add(ray.origin, ray.diff), obbWorldMatrixInverse);

	AABB localAABB{
		{-obb.size.x,-obb.size.y,-obb.size.z},
		{+obb.size.x,+obb.size.y,+obb.size.z}
	};

	Ray localRay;
	localRay.origin = localOrigin;
	localRay.diff = Subtract(localEnd, localOrigin);

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

	Vector3 localOrigin = Transform(segment.origin, obbWorldMatrixInverse);
	Vector3 localEnd = Transform(Add(segment.origin, segment.diff), obbWorldMatrixInverse);

	AABB localAABB{
		{-obb.size.x,-obb.size.y,-obb.size.z},
		{+obb.size.x,+obb.size.y,+obb.size.z}
	};

	Segment localSegment;
	localSegment.origin = localOrigin;
	localSegment.diff = Subtract(localEnd, localOrigin);

	return IsCollision(localAABB, localSegment);
}

//OBBとOBBの衝突
bool IsCollision(const OBB& obb1, const OBB& obb2) {

	Matrix4x4 obb1WorldMatrix{
		.m{
			{obb1.orientations[0].x	,obb1.orientations[0].y	,obb1.orientations[0].z	,0.0f},
			{obb1.orientations[1].x	,obb1.orientations[1].y	,obb1.orientations[1].z	,0.0f},
			{obb1.orientations[2].x	,obb1.orientations[2].y	,obb1.orientations[2].z	,0.0f},
			{obb1.center.x			,obb1.center.y			,obb1.center.z			,1.0f},
		}
	};
	Matrix4x4 obb1WorldMatrixInverse = Inverse(obb1WorldMatrix);

	Matrix4x4 obb2WorldMatrix{
		.m{
			{obb2.orientations[0].x	,obb2.orientations[0].y	,obb2.orientations[0].z	,0.0f},
			{obb2.orientations[1].x	,obb2.orientations[1].y	,obb2.orientations[1].z	,0.0f},
			{obb2.orientations[2].x	,obb2.orientations[2].y	,obb2.orientations[2].z	,0.0f},
			{obb2.center.x			,obb2.center.y			,obb2.center.z			,1.0f},
		}
	};
	Matrix4x4 obb2WorldMatrixInverse = Inverse(obb2WorldMatrix);

	AABB localAABB{
		{-obb1.size.x,-obb1.size.y,-obb1.size.z},
		{+obb1.size.x,+obb1.size.y,+obb1.size.z}
	};

	float min1 = std::min(localAABB.min.x, localAABB.max.x);

	return false;
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