#pragma once
#include "CollisionID.h"
#include <Shape/Capsule.h>
#include <Matrix4x4.h>
#include <stdint.h>
#include <memory>

struct CapsuleCollider {
	//ローカル座標
	Capsule localCapsule;
	//ペアレント座標
	std::shared_ptr<Matrix4x4> parentMatrix;
	//接触範囲
	Capsule colliderCapsule;
	//自分->相手
	uint8_t sourceId;
	//相手->自分
	uint8_t targetId;
};