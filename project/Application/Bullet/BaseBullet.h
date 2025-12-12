#pragma once
#include <memory>
#include <Object/Object.h>
#include <Camera/Camera.h>
#include <ModelHolder/ModelHolder.h>
#include "../Collider/Collider.h"

class BaseBullet : public Collider {
protected:
	std::unique_ptr<Object> object_;
	SRT transform_;
	SRT velocity_;
public:
	virtual void Initialize();
	virtual void Update();
	virtual void Draw();

	virtual void IsCollision() {}

	void InitializeCollider(float radius, uint8_t id) { radius_ = radius; id_ = id; }
	void UpdateCollider() { sphere_.center = transform_.translate; sphere_.radius = radius_ * transform_.scale.x; }
};