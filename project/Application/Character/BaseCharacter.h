#pragma once
#include <memory>
#include <Object/Object.h>
#include <ModelHolder/ModelHolder.h>
#include <Collider/Colliders.h>

class BaseCharacter
{
protected:
	//自キャラモデル
	std::unique_ptr<Object> object_;
	SRT transform_;
	Vector3 velocity_;
	Vector3 acceleration_;

	//コライダー座標
	std::shared_ptr<Matrix4x4> colliderParent_;
	//コライダー
	std::unique_ptr<Colliders> colliders_;

public:
	//初期化
	void Initialize(Vector3 size, CollisionID id);
	virtual void Initialize();
	//更新
	virtual void Update();
	//描画
	virtual void Draw();

	virtual void IsCollision(uint8_t targetId) { colliders_->IsCollision(targetId); }

	Colliders* GetColliders() {	return colliders_.get();}
};

