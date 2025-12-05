#pragma once
#include <memory>
#include "../BaseCharacter.h"
#include "Shape/Sphere.h"
#include "ParticleEmitter/ParticleEmitter.h"

class GameScene;

class Player : public BaseCharacter
{
private:
	float angle;
	Vector3 move_;
	const float deadZone = 0.7f;
	bool isMove = false;
	const float speed = 0.6f;

	std::unique_ptr<SRT> targetTransform_;
	SRT* bossTransform_ = nullptr;

	const SRT* cameraTransform_{};

	enum class DODGE_ANGLE {
		FRONT,
		BACK,
		RIGHT,
		LEFT,
	};

	//回避間隔
	const float kMaxDodgeCoolTime = 0.2f;
	float dodgeCoolTime = 0.0f;
	//回避時間
	const float kMaxDodgeActiveTime = 0.4f;
	float dodgeActiveTime = 0.0f;
	DODGE_ANGLE dodgeAngle_;
	const float dodgeSpeed = 3.0f;

	const float kMaxHP = 3.0f;
	float HP_;

	//残弾数
	const int kMaxRemainingRounds = 6;
	int remainingRounds_;

	//リロード時間
	const float kMaxReloadTime = 2.0f;
	float reloadTime_;

	//発射間隔
	const float kMaxShotCooltime = 0.25f;
	float shotCooltime_ = 0.0f;

	//気絶時間
	const float kMaxHitFenceStunTime = 0.2f;
	float stunTime = 0.0f;

	GameScene* gameScene_ = nullptr;

	ParticleEmitter* particle_1 = nullptr;
	std::unique_ptr<ParticleEmitter> particle_2;
	Emitter emitter_;

	bool isTargeted_ = false;

public:

	~Player();

	//初期化
	void Initialize(ModelHolder* modelHolder, GameScene* gameScene, ParticleEmitter* particle1);
	//更新
	void Update();
	//描画
	void Draw();

	void IsCollision();

	SRT* GetTransform() { return targetTransform_.get(); }
	void SetBossTransform(SRT* bossTransform) { bossTransform_ = bossTransform; }

	void SetCameraTransform(const SRT* transform) { cameraTransform_ = transform; }

	void SetCamera(Camera* camera) { object_->SetCamera(camera); }

	void SetDirectionalLight(DirectionalLight* directionalLight) { object_->SetDirectionalLight(directionalLight); }
	void SetPointLight(PointLight* pointLight) { object_->SetPointLight(pointLight); }

	bool GetIsTargeted() { return isTargeted_; }
	bool IsDead() { return HP_ <= 0; }

	Vector3 GetMove() { return move_; }
};

