#pragma once
#include "../BaseBullet.h"

class PlayerBullet : public BaseBullet {
private:
	static Camera* camera_;
	static DirectionalLight* directionalLight_;
	static PointLight* pointLight_;
	const float speed_ = 1.5f;

	const float kMaxLifeTime_ = 1.5f;
	float lifeTime_ = 0.0f;

	bool isDead = false;
public:
	//初期化
	void Initialize(ModelHolder* modelHolder, Vector3 translate, Vector3 rotate);
	//更新処理
	void Update();
	//描画処理
	void Draw();

	void IsCollision();

	static void SetCamera(Camera* camera) { camera_ = camera; }
	static void SetDirectionalLight(DirectionalLight* directionalLight) { directionalLight_ = directionalLight; }
	static void SetPointLight(PointLight* pointLight) { pointLight_ = pointLight; }

	bool IsDead() { return isDead; }
};