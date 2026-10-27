#pragma once
#include "CollisionID.h"
#include "Shape/OBB.h"
#include "Matrix4x4.h"
#include <cstdint>
#include <memory>

struct OBBCollider {
	//ローカル座標
	OBB localOBB;
	//ペアレント座標
	std::shared_ptr<Matrix4x4> parentMatrix;
	//接触範囲
	OBB colliderOBB;
	//自分->相手
	uint8_t sourceId;
	//相手->自分
	uint8_t targetId;
};