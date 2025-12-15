#pragma once
#include "../BaseBullet.h"

class BossBullet : public BaseBullet {
private:
	static shared_ptr<Camera> camera_;
	static shared_ptr<DirectionalLight> directionalLight_;
	static shared_ptr<PointLight> pointLight_;
	const float speed_ = 1.5f;

	const float kMaxLifeTime_ = 1.5f;
	float lifeTime_ = 0.0f;

	bool isDead = false;
public:
	//初期化
	void Initialize(Vector3 translate, Vector3 rotate);
	//更新処理
	void Update();
	//描画処理
	void Draw();

	void IsCollision();

	static void SetCamera(shared_ptr<Camera> camera) { camera_ = camera; }
	static void SetDirectionalLight(shared_ptr<DirectionalLight> directionalLight) { directionalLight_ = directionalLight; }
	static void SetPointLight(shared_ptr<PointLight> pointLight) { pointLight_ = pointLight; }

	bool IsDead() { return isDead; }
};