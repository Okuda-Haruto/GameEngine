#pragma once
#include <Collider/OBBCollider.h>
#include <Object/Object.h>
#include <Collider/Colliders.h>

/// <summary>
/// 接触可能なオブジェクト
/// </summary>
class ColliderObject : public Collider {
private:
	std::unique_ptr<Object> object_;

	std::shared_ptr<Colliders> colliders_;

	//コライダー座標
	std::shared_ptr<Matrix4x4> colliderParent_;

	SRT transform_;
public:
	//初期化
	void Initialize(std::shared_ptr<Model> model, SRT transform);
	//更新処理
	void Update();
	//描画処理
	void Draw();

	virtual void IsCollision(uint8_t targetId) override {}

	virtual void IsCollisionGround(OBB obb) override {}

	std::shared_ptr<Colliders> GetCollider() { return colliders_; }
};