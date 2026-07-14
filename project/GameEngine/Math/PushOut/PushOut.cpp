#include "PushOut.h"
#include <Operation/Operation.h>
#include <Collision.h>

Vector3 PushOut(Sphere& sphere, const Vector3& velocity, const OBB& obb) {
	Matrix4x4 obbWorldMatrix{
		.m{
			{obb.orientations[0].x	,obb.orientations[0].y	,obb.orientations[0].z	,0.0f},
			{obb.orientations[1].x	,obb.orientations[1].y	,obb.orientations[1].z	,0.0f},
			{obb.orientations[2].x	,obb.orientations[2].y	,obb.orientations[2].z	,0.0f},
			{obb.center.x			,obb.center.y			,obb.center.z			,1.0f},
		}
	};
	Matrix4x4 obbWorldMatrixInverse = Inverse(obbWorldMatrix);

	//最近接点
	Vector3 closest = ClosestPoint(sphere.center, obb);

	//法線から押し出し方向を得る
	Vector3 localNormal = sphere.center * obbWorldMatrixInverse - closest;
	float length = Length(localNormal);
	if (length > 0.0f)
	{
		localNormal /= length;
	}

	//位置情報が入らないようにワールド空間へ
	Vector3 normal =
		localNormal.x * obb.orientations[0] +
		localNormal.y * obb.orientations[1] +
		localNormal.z * obb.orientations[2];

	//押し出し
	float penetration = sphere.radius - length;
	if (penetration > 0.0f)
	{
		sphere.center += normal * penetration;
	}

	//壁沿いに進む場合のvelocity
	Vector3 slide = velocity - normal * Dot(velocity, normal);

	return slide;
}