#pragma once
#include <Object/Object.h>
#include <Camera/Camera.h>
#include <ModelHolder/ModelHolder.h>
#include <Collider/Colliders.h>

class BaseBullet : public Collider {
protected:
	std::unique_ptr<Object> object_;
	SRT transform_;
	SRT velocity_;

	//コライダー座標
	std::shared_ptr<Matrix4x4> colliderParent_;
	//コライダー
	std::unique_ptr<Colliders> colliders_;
public:

	void Initialize(float radius, CollisionID id);
	virtual void Update();
	virtual void Draw();

	virtual void IsCollision(uint8_t targetId) override {}

	virtual void IsCollisionGround(OBB obb) override {}

	Colliders* GetColliders() { return colliders_.get(); }
};