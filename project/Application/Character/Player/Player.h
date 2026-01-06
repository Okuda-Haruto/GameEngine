#pragma once
#include <memory>
#include "../BaseCharacter.h"
#include "Shape/Sphere.h"
#include "GameCamera/GameCamera.h"
#include "ParticleEmitter/ParticleEmitter.h"

using namespace std;

class GameScene;

class Player : public BaseCharacter
{
private:
	shared_ptr<Input> input_;

	float angle;
	Vector3 move_;
	const float deadZone = 0.7f;
	bool isMove = false;
	const float kMaxSpeed = 0.6f;
	const float speed = 0.06f;

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

	const int32_t kMaxHP = 5;
	int32_t HP_;

	//残弾数
	const int32_t kMaxRemainingRounds = 6;
	int32_t remainingRounds_;

	//リロード時間
	const float kMaxReloadTime = 1.0f;
	float reloadTime_;

	//発射間隔
	const float kMaxShotCooltime = 0.5f;
	float shotCooltime_ = 0.0f;
	
	//気絶時間
	const float kMaxHitFenceStunTime = 0.2f;
	float stunTime = 0.0f;

	//無敵時間
	const float kMaxInvincibleTime_ = 0.5f;
	float invincibleTime_ = 0.0f;

	//停止時間
	const float kMaxStopTime_ = 1.2f;
	float stopTime_ = 0.0f;

	GameScene* gameScene_ = nullptr;
	GameCamera* gameCamera_ = nullptr;

	ParticleEmitter* particle_1 = nullptr;
	std::unique_ptr<ParticleEmitter> particle_2;
	Emitter emitter_;

	bool isTargeted_ = false;

public:

	~Player();

	//初期化
	void Initialize(GameScene* gameScene, GameCamera* gameCamera, shared_ptr<Input> input, ParticleEmitter* particle1);
	//更新
	void Update();
	//描画
	void Draw();

	void IsCollision();

	SRT* GetTransform() { return targetTransform_.get(); }
	void SetBossTransform(SRT* bossTransform) { bossTransform_ = bossTransform; }

	void SetCameraTransform(const SRT* transform) { cameraTransform_ = transform; }

	void SetCamera(shared_ptr<Camera> camera) { object_->SetCamera(camera); }

	void SetDirectionalLight(shared_ptr<DirectionalLight> directionalLight) { object_->SetDirectionalLight(directionalLight); }
	void SetPointLight(shared_ptr<PointLight> pointLight) { object_->SetPointLight(pointLight); }

	bool GetIsTargeted() { return isTargeted_; }
	bool IsDead() { return HP_ <= 0; }

	int32_t GetHP() { return HP_; }
	int32_t GetRemainingRounds() { return remainingRounds_; }

	Vector3 GetMove() { return move_; }
};

