#pragma once
#include "Shape/Sphere.h"
#include <stdint.h>

// 1桁目	プレイヤー側か
// 2桁目	ボス側か
// 3桁目	キャラクターか
enum CollisionID {
	CollisionID_Player_Character = 0b101,
	CollisionID_Player_Bullet = 0b001,
	CollisionID_Enemy_Character = 0b110,
	CollisionID_Enemy_Bullet = 0b010,
	CollisionID_Item_Character = 0b100,
	CollisionID_Item_Bullet = 0b000,
};

class Collider {
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