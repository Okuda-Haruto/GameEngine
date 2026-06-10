#pragma once
#include "SphereCollider.h"
#include "OBBCollider.h"
#include "Collider.h"
#include <vector>
#include <memory>

//コライダーをまとめたもの
class Colliders {
private:
	//接触判定
	std::vector<SphereCollider> sphereColliders_;
	std::vector<OBBCollider> obbColliders_;

	//対地面接触判定
	OBBCollider grundCollider_;

	//接触した判定を伝えるclass
	Collider* collider_;

public:
	//初期化
	void Initialize(Collider* collider);
	//更新処理
	void Update();

	// 接触された判定
	void IsCollision(uint8_t sourceId) { collider_->IsCollision(sourceId); }

	// 球接触判定の追加
	void AddSphereCollider(Sphere localSphere, int8_t sourceId_, int8_t targetId_, std::shared_ptr<Matrix4x4> parentMatrix);

	// OBB接触判定の追加
	void AddOBBCollider(OBB localOBB, int8_t sourceId_, int8_t targetId_, std::shared_ptr<Matrix4x4> parentMatrix);

	//対地面接触判定
	void SetGrundCollider(OBB localOBB, int8_t sourceId_, int8_t targetId_);

	//球接触判定の変更
	std::vector<SphereCollider> GetSphereColliders() { return sphereColliders_; }
	void SetSphereColliders(int32_t index, SphereCollider sphereCollider) { sphereColliders_[index] = sphereCollider; }

	//OBB接触判定
	std::vector<OBBCollider> GetOBBColliders() { return obbColliders_; }
	void SetOBBColliders(int32_t index, OBBCollider obbCollider) { obbColliders_[index] = obbCollider; }
};