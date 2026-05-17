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

	//地面接触判定
	OBBCollider grundCollider_;

	//
	Collider* collider_;

public:
	//初期化
	void Initialize(OBBCollider grundCollider);
	//更新処理
	void Update();

	// 接触された判定
	virtual void IsCollision(uint8_t sourceId) = 0;

	// 球接触判定の追加
	void AddSphereCollider(Sphere localSphere, int8_t sourceId_, int8_t targetId_);

	// OBB接触判定の追加
	void AddOBBCollider(OBB localOBB, int8_t sourceId_, int8_t targetId_);

	//球接触判定
	std::vector<SphereCollider> GetSphereColliders() { return sphereColliders_; }
	//OBB接触判定
	std::vector<OBBCollider> GetOBBColliders() { return obbColliders_; }
};