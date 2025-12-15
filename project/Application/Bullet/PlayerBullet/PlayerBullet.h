#pragma once
#include "../BaseBullet.h"

class PlayerBullet : public BaseBullet {
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
	void Initialize(Vector3 translate, Vector3 rotate);
	//更新処理
	void Update();
	//描画処理
	void Draw();

	void IsCollision();

	static void SetCamera(weak_ptr<Camera> camera) { camera_ = camera; }
	static void SetDirectionalLight(weak_ptr<DirectionalLight> directionalLight) { directionalLight_ = directionalLight; }
	static void SetPointLight(weak_ptr<PointLight> pointLight) { pointLight_ = pointLight; }

	bool IsDead() { return isDead; }
};