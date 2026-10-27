#pragma once
#include "../BaseEntity.h"
#include "PrimitiveManager/PrimitiveCylinder.h"

class ShockWave : public BaseEntity {
private:
	static std::weak_ptr<Camera> camera_;
	static std::weak_ptr<DirectionalLight> directionalLight_;
	static std::weak_ptr<PointLight> pointLight_;

	float range_;

	std::unique_ptr<PrimitiveCylinder> cylinder_;
	Material cylinderMaterial_;

	float maxLifeTime_;
	float lifeTime_ = 0.0f;

	bool isDead = false;
public:
	//初期化
	void Initialize(SRT transform, float range, float maxLifeTime, CollisionID id, std::unique_ptr<PrimitiveCylinder> cylinder);
	//更新処理
	void Update();
	//描画処理
	void Draw();

	void IsCollision(uint8_t targetId) override;

	static void SetCamera(std::weak_ptr<Camera> camera) { camera_ = camera; }
	static void SetDirectionalLight(std::weak_ptr<DirectionalLight> directionalLight) { directionalLight_ = directionalLight; }
	static void SetPointLight(std::weak_ptr<PointLight> pointLight) { pointLight_ = pointLight; }

	bool IsDead() { return isDead; }
};