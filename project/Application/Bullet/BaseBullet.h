#pragma once
#include <Object/Object.h>
#include <Camera/Camera.h>
#include <ModelHolder/ModelHolder.h>
#include <Collider/Colliders.h>

class BaseBullet{
protected:
	std::unique_ptr<Object> object_;
	SRT transform_;
	SRT velocity_;

	//コライダー座標
	std::shared_ptr<Matrix4x4> colliderParent_;
	//コライダー
	std::unique_ptr<Colliders> colliders_;
public:

	void Initialize(Vector3 size, CollisionID id);
	virtual void Initialize();
	virtual void Update();
	virtual void Draw();

	virtual void IsCollision(uint8_t targetId) { colliders_->IsCollision(targetId); }

	Colliders* GetColliders() { return colliders_.get(); }
};