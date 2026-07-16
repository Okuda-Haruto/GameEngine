#pragma once
#include "../BaseEntity.h"

class Item : public BaseEntity {
private:
	static weak_ptr<Camera> camera_;
	static weak_ptr<DirectionalLight> directionalLight_;
	static weak_ptr<PointLight> pointLight_;
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

	static void SetCamera(weak_ptr<Camera> camera) { camera_ = camera; }
	static void SetDirectionalLight(weak_ptr<DirectionalLight> directionalLight) { directionalLight_ = directionalLight; }
	static void SetPointLight(weak_ptr<PointLight> pointLight) { pointLight_ = pointLight; }

	bool IsDead() { return isDead; }
};