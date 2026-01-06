#pragma once
#include <memory>
#include "../BaseCharacter.h"
#include "../Player/Player.h"
#include "GameCamera/GameCamera.h"
#include "ParticleEmitter/ParticleEmitter.h"

using namespace std;

class GameScene;
class Boss;

class BossAction {
protected:
	SRT startTransform;
	Boss* boss_ = nullptr;
	bool isEnd_ = false;

public:
	virtual ~BossAction() = default; // 追加: 仮想デストラクタ
	virtual void Initialize(Boss* boss) = 0;
	virtual void Update() = 0;
	virtual void Finalize() = 0;
	bool IsEnd() { return isEnd_; }
};

class BossAction_Shot_01 : public BossAction {
private:
	//行動に要する総時間
	const float kMaxActionTime_ = 2.0f;
	float actionTime_ = 0.0f;

	//構えてから撃つ時間
	const float kMaxShotCooltime_ = 0.2f;
	float shotCooltime_ = 0.0f;

	//狙って構えるまでの時間
	const float kMaxTargetedCooltime_ = 0.3f;
	float targetedCooltime_ = 0.0f;

	bool isTargeted_ = false;

public:
	void Initialize(Boss* boss) override;
	void Update() override;
	void Finalize() override;
};

class BossAction_Shot_02 : public BossAction {
private:
	//行動に要する総時間
	const float kMaxActionTime_ = 1.0f;
	float actionTime_ = 0.0f;

	//構えてから撃つ時間
	const float kMaxShotCooltime_ = 0.1f;
	float shotCooltime_ = 0.0f;

	//狙って構えるまでの時間
	const float kMaxTargetedCooltime_ = 0.2f;
	float targetedCooltime_ = 0.0f;

	bool isTargeted_ = false;

public:
	void Initialize(Boss* boss) override;
	void Update() override;
	void Finalize() override;
};

class Boss : public BaseCharacter
{
private:

	float angle;

	std::unique_ptr<SRT> targetTransform_;

	Vector4 color_;

	float maxHP_;
	float HP_;

	GameCamera* gameCamera_ = nullptr;
	ParticleEmitter* particle_ = nullptr;

	GameScene* gameScene_ = nullptr;
	Player* player_ = nullptr;

	std::unique_ptr<BossAction> action_;

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
	GameScene* GetGameScene() { return gameScene_; }
	Player* GetPlayer() { return player_; }

	void SetCamera(shared_ptr<Camera> camera) { object_->SetCamera(camera); }

	void SetDirectionalLight(shared_ptr<DirectionalLight> directionalLight) { object_->SetDirectionalLight(directionalLight); }
	void SetPointLight(shared_ptr<PointLight> pointLight) { object_->SetPointLight(pointLight); }

	bool IsDead() { return HP_ <= 0; }
};

