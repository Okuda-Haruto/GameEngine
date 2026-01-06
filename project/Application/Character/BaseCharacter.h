#pragma once
#include <memory>
#include <Object/Object.h>
#include <ModelHolder/ModelHolder.h>
#include "../Collider/Collider.h"

class BaseCharacter : public Collider
{
protected:
	//自キャラモデル
	std::unique_ptr<Object> object_;
	SRT transform_;
	Vector3 velocity_;
	Vector3 acceleration_;

public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();

	virtual void IsCollision() {}

	void InitializeCollider(float radius, uint8_t id) { radius_ = radius; id_ = id; }
	void UpdateCollider() { sphere_.center = transform_.translate; sphere_.radius = radius_ * transform_.scale.x; }
	void SetInvincible(bool invincible) { invincible_ = invincible; }
};

