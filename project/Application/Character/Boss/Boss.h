#pragma once
#include <memory>
#include "../BaseCharacter.h"
#include "../Player/Player.h"
#include "GameCamera/GameCamera.h"
#include "ParticleEmitter/ParticleEmitter.h"

class Boss;
class BossAction;

struct LerpPositionState {
	//開始地点
	Vector3 startVector;
	//終了地点
	Vector3 endVector;
	//補完時間
	float time;
	//補完タイプ
	int32_t type;
};
struct VelocityState {
	//速度
	Vector3 velocity;
	//時間
	float time;
};

#pragma region Step

//1つ単位の行動
class BaseStep {
private:

public:
	virtual void Activate(BossAction* action) = 0;
};

//指定時間待つ
class Step_WaitTime : public BaseStep {
private:
	float time_;
public:
	Step_WaitTime(float time) { time_ = time; }
	void Activate(BossAction* action) override;
};
//他同時進行中のステップが終わるまで待つ
class Step_WaitStep : public BaseStep {
private:

public:
	void Activate(BossAction* action) override;
};
//アニメーション終了まで待つ
class Step_WaitAnimation : public BaseStep {
private:

public:
	void Activate(BossAction* action) override;
};

//指定時間で指定位置に移動する
class Step_MoveFixedPsitionTime : public BaseStep {
private:
	float time_;
	Vector3 position_;
public:
	Step_MoveFixedPsitionTime(Vector3 position, float time) { position_ = position;  time_ = time; }
	void Activate(BossAction* action) override;
};
//指定速度で指定位置に移動する
class Step_MoveFixedPsitionSpeed : public BaseStep {
private:
	float speed_;
	Vector3 position_;
public:
	Step_MoveFixedPsitionSpeed(Vector3 position, float speed) { position_ = position;  speed_ = speed; }
	void Activate(BossAction* action) override;
};
//指定速度で移動する
class Step_MoveFixedVelocity : public BaseStep {
private:
	Vector3 velocity_;
	float time_;
public:
	Step_MoveFixedVelocity(Vector3 velocity, float time) { velocity_ = velocity;  time_ = time; }
	void Activate(BossAction* action) override;
};
//向いてる方向に移動する
class Step_MoveFront : public BaseStep {
private:
	float speed_;
	float time_;
public:
	Step_MoveFront(float speed, float time) { speed_ = speed;  time_ = time; }
	void Activate(BossAction* action) override;
};
//LockOn対象に向けて移動する
class Step_MoveToLockOn : public BaseStep {
private:
	float speed_;
	float time_;
public:
	Step_MoveToLockOn(float speed, float time) { speed_ = speed;  time_ = time; }
	void Activate(BossAction* action) override;
};

//プレイヤーをロックオンする
class Step_LockOnPlayer : public BaseStep {
private:

public:
	void Activate(BossAction* action) override;
};
//ロックオン解除
class Step_LockOnRelease : public BaseStep {
private:

public:
	void Activate(BossAction* action) override;
};

//向いてる方向に弾発射
class Step_ShotBulletToFront : public BaseStep {
private:
	float spread_;
	float speed_;
public:
	Step_ShotBulletToFront(float spread, float speed) { spread_ = spread; speed_ = speed; }
	void Activate(BossAction* action) override;
};

#pragma endregion

//ボスのアクション
class BossAction {
private:
	Boss* boss_;
	//ボス行動ステップ
	std::vector<std::unique_ptr<BaseStep>> steps_;
	int stepIndex_;

	//待ち時間
	float waitTime_;
	//Step待ち
	bool isWaitStep_;
	//アニメーション待ち
	bool isWaitAnimation_;
	//ロックオン
	bool isLockOnPlayer_;

	//線形補完位置
	std::optional<LerpPositionState> lerpPosition_;
	//線形補完移動
	std::optional<VelocityState> velocityState_;

	//終了
	bool isEnd_;

public:
	void Initialize(Boss* boss) { boss_ = boss; isEnd_ = false; stepIndex_ = 0; }
	void Update();

	Boss* GetBoss() { return boss_; }

	bool IsEnd() { return isEnd_; }

	//ステップ
	std::vector<BaseStep*> GetSteps() {
		std::vector<BaseStep*> result;
		for (auto& step : steps_) {
			result.push_back(step.get());
		}
		return result;
	}
	void SetSteps(std::vector<std::unique_ptr<BaseStep>> steps) { steps_ = move(steps); }

	//待ち時間
	float GetWaitTime() { return waitTime_; }
	void SetWaitTime(float time) { waitTime_ = time; }

	//Step待ち
	bool GetWaitStep() { return isWaitStep_; }
	void SetWaitStep(bool isWaitStep) { isWaitStep_ = isWaitStep; }

	//アニメーション待ち
	bool GetWaitAnimation() { return isWaitAnimation_; }
	void SetWaitAnimation(bool isWaitAnimation) { isWaitAnimation_ = isWaitAnimation; }

	//待ち状態か
	bool GetIsStop() { return (waitTime_ > 0.0f || isWaitStep_ || isWaitAnimation_); }

	//プレイヤーをロックオン中か
	float GetIsLockOnPlayer() { return isLockOnPlayer_; }
	void SetIsLockOnPlayer(bool isLockOn) { isLockOnPlayer_ = isLockOn; }

	//線形補完位置
	std::optional<LerpPositionState> GetLerpPosition() { return lerpPosition_; }
	void SetLerpPosition(LerpPositionState lerpPosition) { lerpPosition_ = lerpPosition; }
	void ResetLerpPosition() { lerpPosition_.reset(); }

	//線形補完移動
	std::optional<VelocityState> GetVelocityState() { return velocityState_; }
	void SetVelocityState(VelocityState velocityState) { velocityState_ = velocityState; }
	void ResetVelocityState() { velocityState_.reset(); }
};

//パターン条件
struct PatternCondition {
	//体力割合(1.0f~0.0f)
	std::optional<float> minHpRate;
	std::optional<float> maxHpRate;
	//プレイヤーとの距離(近い場合判定)
	std::optional<float> nearDistance;
	//プレイヤーとの距離(遠い場合判定)
	std::optional<float> farDistance;
	//優先度(9~1)
	int8_t priority;
};

//ボスの行動パターン
class BossPattern {
private:
	Boss* boss_;
	//パターン条件
	PatternCondition condition_;
	//ボス行動
	std::unique_ptr<BossAction> action_;

	//終了
	bool isEnd_;
public:
	void Initialize(Boss* boss) { boss_ = boss; isEnd_ = false; action_->Initialize(boss); }
	void Update();

	bool IsEnd() { return isEnd_; }

	//パターン条件
	PatternCondition GetCondition() { return condition_; }
	void SetCondition(PatternCondition condition) { condition_ = condition; }

	//ボス行動
	BossAction* GetAction() { return action_.get(); }
	void SetAction(std::unique_ptr<BossAction> action) { action_ = move(action); }
};

class GameScene;

class Boss : public BaseCharacter
{
private:

	float angle;


	std::unique_ptr<SRT> targetTransform_;

	float maxHP_;
	float HP_;

	GameCamera* gameCamera_ = nullptr;
	ParticleEmitter* particle_ = nullptr;

	GameScene* gameScene_ = nullptr;
	Player* player_ = nullptr;

	std::vector<std::unique_ptr<BossPattern>> patterns_;
	uint32_t patternIndex_;

	//線形補完位置
	std::optional<LerpPositionState> lerpPosition_{};
	float lerpPositionTime_;
	//線形補完移動
	std::optional<VelocityState> velocityState_{};
	float velocityStateTime_;

	bool isStartAnimation_ = false;

	Vector3 lockOnPosition_;

	bool isAction_;

public:

	~Boss();

	//初期化
	void Initialize(GameScene* gameScene, GameCamera* gameCamera, ParticleEmitter* particle, Player* player, float maxHP);
	//更新
	void Update();
	//描画
	void Draw();

	void IsCollision(uint8_t targetId) override;

	void IsCollisionGround(OBB obb) override;

	SRT* GetTransform() { return targetTransform_.get(); }
	SRT* GetPlayerTransform() { return player_->GetTransform(); }
	Vector3 GetVelocity() { return velocity_; }
	GameScene* GetGameScene() { return gameScene_; }
	Player* GetPlayer() { return player_; }
	GameCamera* GetGameCamera() { return gameCamera_; }

	void SetCamera(shared_ptr<Camera> camera) { object_->SetCamera(camera); }

	void SetDirectionalLight(shared_ptr<DirectionalLight> directionalLight) { object_->SetDirectionalLight(directionalLight); }
	void SetPointLight(shared_ptr<PointLight> pointLight) { object_->SetPointLight(pointLight); }

	bool IsDead() { return HP_ <= 0; }

	bool IsStartAnimation() { return isStartAnimation_; }

	bool IsAction() { return isAction_; }

	void ShotBullet(Vector3 startPoint, Vector3 rotate, float speed);
};

