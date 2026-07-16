#pragma once
#include "../BaseEntity.h"
#include <Event/Event.h>

class Bullet : public BaseEntity {
public:
	enum class BulletMove {
		NormalBullet,
		FallingBullet,
		HomingBullet
	};
private:
	static weak_ptr<Camera> camera_;
	static weak_ptr<DirectionalLight> directionalLight_;
	static weak_ptr<PointLight> pointLight_;

	//弾の動かし方
	BulletMove bulletMove_;

	//速度
	float speed_;
	//死亡時行動
	std::unique_ptr<BaseEvent> deadEvent_;

	const float kMaxLifeTime_ = 1.5f;
	float lifeTime_ = 0.0f;

	bool isDead_ = false;
public:
	//初期化
	void Initialize(SRT transform, float speed, CollisionID id, Bullet::BulletMove bulletMove, std::shared_ptr<Model> model, std::unique_ptr<BaseEvent> event);
	//更新処理
	void Update();
	//描画処理
	void Draw();

	void IsCollision(uint8_t targetId) override;

	void IsCollisionGround(OBB obb) override;

	static void SetCamera(weak_ptr<Camera> camera) { camera_ = camera; }
	static void SetDirectionalLight(weak_ptr<DirectionalLight> directionalLight) { directionalLight_ = directionalLight; }
	static void SetPointLight(weak_ptr<PointLight> pointLight) { pointLight_ = pointLight; }

	bool IsDead() { return isDead_; }
};