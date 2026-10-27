#pragma once
#include "../BaseEntity.h"

class Item : public BaseEntity {
private:
	static std::weak_ptr<Camera> camera_;
	static std::weak_ptr<DirectionalLight> directionalLight_;
	static std::weak_ptr<PointLight> pointLight_;
	const float speed_ = 1.5f;

	const float kMaxLifeTime_ = 1.5f;
	float lifeTime_ = 0.0f;

	bool isDead = false;
public:
	//初期化
	void Initialize(SRT transform);
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