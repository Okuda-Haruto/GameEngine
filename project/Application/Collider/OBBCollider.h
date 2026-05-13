#pragma once
#include "CollisionID.h"
#include "Shape/OBB.h"
#include <SRT.h>
#include <stdint.h>

class OBBCollider {
protected:
	OBB obb_;
	uint8_t id_;
	bool isCollision_;
	//ローカル座標
	SRT localTransform_;
public:
	void IsCollision() {}
	OBB GetOBB() { return obb_; }
	uint8_t GetID() { return id_; }
};