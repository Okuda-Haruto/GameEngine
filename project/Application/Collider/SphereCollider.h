#pragma once
#include "CollisionID.h"
#include "Shape/Sphere.h"
#include <Matrix4x4.h>
#include <stdint.h>
#include <memory>

struct SphereCollider {
	//ローカル座標
	Sphere localSphere_;
	//ペアレント座標
	std::shared_ptr<Matrix4x4> parentMatrix_;
	//接触範囲
	Sphere colliderSphere_;
	//自分->相手
	uint8_t sourceId_;
	//相手->自分
	uint8_t targetId_;
};