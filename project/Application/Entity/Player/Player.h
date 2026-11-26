#pragma once
#include <memory>
#include "../BaseEntity.h"
#include "Shape/Sphere.h"
#include "GameCamera/GameCamera.h"
#include "ParticleEmitter/ParticleEmitter.h"

class Stage;
class Player;

class BasePlayerState {
protected:
	std::weak_ptr<Input> input_;
	Player* player_;

public:
	virtual void Initialize(Player* player, std::weak_ptr<Input> input) = 0;
	virtual void Update() = 0;
};

class PlayerNormalState : public BasePlayerState {
private:
	//移動デッドゾーン
	const float deadZone = 0.7f;
	//最大リロード時間
	const float kMaxReloadTime = 1.0f;
	//最大発射間隔
	const float kMaxShotCooltime = 0.5f;
	//最大回避間隔
	const float kMaxDodgeCoolTime = 0.2f;
	//最大停止時間
	const float kMaxStopTime = 1.2f;

	//リロード時間
	float reloadTime_;
	//発射間隔
	float shotCooltime_;
	//回避間隔
	float dodgeCoolTime_;
	//停止時間
	float stopTime_;
public:
	void Initialize(Player* player, std::weak_ptr<Input> input) override;
	void Update() override;
};

class PlayerDodgeState : public BasePlayerState {
public:
	enum class DODGE_ANGLE {
		FRONT,
		BACK,
		RIGHT,
		LEFT,
	};

private:
	//最大回避時間
	const float kMaxDodgeActiveTime = 0.4f;

	//回避時間
	float dodgeActiveTime_;

	//回避方向
	DODGE_ANGLE dodgeAngle_;
public:
	void Initialize(Player* player, std::weak_ptr<Input> input) override;
	void Update() override;
};

class PlayerStunState : public BasePlayerState {
private:
	//最大気絶時間
	const float kMaxStunTime = 0.2f;

	//気絶時間
	float stunTime_;

public:
	void Initialize(Player* player, std::weak_ptr<Input> input) override;
	void Update() override;
};


class Player : public BaseEntity
{
private:

	//プレイヤーの向き
	float facingAngle_;
	//プレイヤーの移動方向入力
	Vector3 movementInput_;

	//回避移動
	Vector3 dodgeVelocity_;

	std::shared_ptr<Input> input_;

	const float kMaxSpeed = 0.6f;
	const float speed = 0.06f;
	const float dodgeSpeed = 3.0f;

	//カメラ追従用Transform
	std::unique_ptr<SRT> trackingTransform_;

	const SRT* cameraTransform_{};

	std::unique_ptr<ParticleEmitter> moveParticle_ = nullptr;
	std::unique_ptr<ParticleEmitter> damageParticle_ = nullptr;

	const int32_t kMaxHP = 5;
	int32_t HP_;

	//残弾数
	const int32_t kMaxRemainingRounds = 6;
	int32_t remainingRounds_;

	//無敵時間
	const float kMaxInvincibleTime_ = 0.5f;
	float invincibleTime_ = 0.0f;

	Stage* stage_ = nullptr;
	std::shared_ptr<GameCamera> gameCamera_ = nullptr;

	bool isTargeted_ = false;

	//プレイヤー行動ステート
	std::unique_ptr<BasePlayerState> state_;
	//次の行動ステート
	std::unique_ptr<BasePlayerState> nextState_;

public:

	~Player();

	//初期化
	void Initialize(Stage* stage, std::shared_ptr<GameCamera> gameCamera, std::shared_ptr<Input> input, SRT startTransform);
	//更新
	void Update();
	//描画
	void Draw();

	//衝突判定
	void IsCollision(uint8_t targetId) override;

	//移動
	void Move(Vector3 movementInput);
	//回避移動
	void Dodge(PlayerDodgeState::DODGE_ANGLE angle, float easingRate);
	//発射
	void Shot();
	//リロード
	void Reload();
	//スタン
	void Stun();

	//座標
	const SRT* GetTransform() { return trackingTransform_.get(); }
	void SetTransform(SRT transform) { transform_ = transform; *trackingTransform_ = transform_; object_->SetTransform(transform_); }

	//移動方向入力
	Vector3 GetMovementInput() const { return movementInput_; }

	//カメラ座標
	void SetCameraTransform(const SRT* transform) { cameraTransform_ = transform; }

	//使用するカメラ
	void SetCamera(std::weak_ptr<Camera> camera) { object_->SetCamera(camera); }

	//使用する光源
	void SetDirectionalLight(std::shared_ptr<DirectionalLight> directionalLight) { object_->SetDirectionalLight(directionalLight); }
	void SetPointLight(std::shared_ptr<PointLight> pointLight) { object_->SetPointLight(pointLight); }

	//ロックオンしているか
	void SetIsTargeted(bool isTargeted) { isTargeted_ = isTargeted; }
	bool IsTargeted() const { return isTargeted_; }

	//HP
	int32_t GetHP() const { return HP_; }
	//回復
	void AddHP(int32_t hp) { HP_ += hp; if (HP_ > kMaxHP) HP_ = kMaxHP; }
	//死亡判定
	bool IsDead() const { return HP_ <= 0; }

	//残弾数
	int32_t GetRemainingRounds() const { return remainingRounds_; }
	//残弾数が満タンか
	bool IsRemainingRoundsFull() const { return remainingRounds_ == kMaxRemainingRounds; }

	//無敵時間割合
	float GetInvincibleTimeRate() const { return invincibleTime_ / kMaxInvincibleTime_; }

	//次の行動ステート
	void SetNextState(std::unique_ptr<BasePlayerState> state);

#ifdef USE_IMGUI
	//imGuiに載せるステータス
	void ImGuiState();
#endif
};

