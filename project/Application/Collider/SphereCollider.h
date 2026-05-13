#pragma once
#include "CollisionID.h"
#include "Shape/Sphere.h"
#include <stdint.h>

class SphereCollider {
protected:
	bool invincible_ = false;
	float radius_;
	Sphere sphere_;
	uint8_t id_;
public:
	virtual void IsCollision() {}
	Sphere GetSphere() { return sphere_; }
	uint8_t GetID() { return id_; }
	bool GetInvincible() { return invincible_; }
};