#pragma once
#include <memory>
#include "../BaseCharacter.h"
#include "../Player/Player.h"
#include "GameCamera/GameCamera.h"
#include "ParticleEmitter/ParticleEmitter.h"

using namespace std;

class GameScene;

class BossAction {
protected:
	SRT startTransform;

public:
	virtual void Initialize();
	virtual void Update();
	virtual void Finalize();
};

class Boss : public BaseCharacter
{
private:
	const float kMaxShotCooltime = 1.0f;
	float shotCooltime_ = 0.0f;

	float angle;

	std::unique_ptr<SRT> targetTransform_;

	float maxHP_;
	float HP_;

	GameCamera* gameCamera_ = nullptr;
	ParticleEmitter* particle_ = nullptr;

	GameScene* gameScene_ = nullptr;
	Player* player_ = nullptr;

public:

	~Boss();

	//初期化
	void Initialize(GameScene* gameScene, GameCamera* gameCamera, ParticleEmitter* particle, Player* player, float maxHP);
	//更新
	void Update();
	//描画
	void Draw();

	void IsCollision();

	SRT* GetTransform() { return targetTransform_.get(); }

	void SetCamera(shared_ptr<Camera> camera) { object_->SetCamera(camera); }

	void SetDirectionalLight(shared_ptr<DirectionalLight> directionalLight) { object_->SetDirectionalLight(directionalLight); }
	void SetPointLight(shared_ptr<PointLight> pointLight) { object_->SetPointLight(pointLight); }

	bool IsDead() { return HP_ <= 0; }
};

