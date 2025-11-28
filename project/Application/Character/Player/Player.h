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

	std::unique_ptr<SRT> targetTransform_;

	const SRT* cameraTransform_{};

	const float kMaxDodgeCoolTime = 0.2f;
	float dodgeCoolTime = 0.0f;
	const float kMaxDodgeActiveTime = 0.4f;
	float dodgeActiveTime = 0.0f;

	const float kMaxHP = 3.0f;
	float HP_;

	const float kMaxShotCooltime = 0.25f;
	float shotCooltime = 0.0f;

	GameScene* gameScene_ = nullptr;

	ParticleEmitter* particle_1 = nullptr;
	std::unique_ptr<ParticleEmitter> particle_2;
	Emitter emitter_;

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

	void SetCameraTransform(const SRT* transform) { cameraTransform_ = transform; }

	void SetCamera(Camera* camera) { object_->SetCamera(camera); }

	void SetDirectionalLight(DirectionalLight* directionalLight) { object_->SetDirectionalLight(directionalLight); }
	void SetPointLight(PointLight* pointLight) { object_->SetPointLight(pointLight); }

	bool IsDead() { return HP_ <= 0; }
};

