#pragma once
#include <memory>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "../BaseCharacter.h"
#include "../Player/Player.h"
#include "GameCamera/GameCamera.h"
#include "ParticleEmitter/ParticleEmitter.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif

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
	virtual nlohmann::json WriteStep() = 0;
	virtual void ReadStep(const nlohmann::json_abi_v3_12_0::json& stepJson) = 0;
	virtual void EditorItem() = 0;
	virtual std::string GetName() = 0;
};

//指定時間待つ
class Step_WaitTime : public BaseStep {
private:
	float time_;
public:
	void Initialize(float time) { time_ = time; }
	void Activate(BossAction* action) override;

	nlohmann::json WriteStep() override {
		return{
			{"step","Step_WaitTime"},
			{"time",time_},
		};
	}

	void ReadStep(const nlohmann::json_abi_v3_12_0::json& stepJson) override {
		Initialize(stepJson["time"]);
	}

	void EditorItem() override {
#ifdef USE_IMGUI
		ImGui::DragFloat("指定時間", &time_);
#endif
	}
	std::string GetName() override {
		return "Step_WaitTime";
	}
};
//他同時進行中のステップが終わるまで待つ
class Step_WaitStep : public BaseStep {
private:

public:
	void Activate(BossAction* action) override;

	nlohmann::json WriteStep() override {
		return{
			{"step","Step_WaitStep"},
		};
	}

	void ReadStep(const nlohmann::json_abi_v3_12_0::json& stepJson) override {

	}

	void EditorItem() override {

	}
	std::string GetName() override {
		return "Step_WaitStep";
	}
};
//アニメーション終了まで待つ
class Step_WaitAnimation : public BaseStep {
private:

public:
	void Activate(BossAction* action) override;

	nlohmann::json WriteStep() override {
		return{
			{"step","Step_WaitAnimation"},
		};
	}

	void ReadStep(const nlohmann::json_abi_v3_12_0::json& stepJson) override {

	}

	void EditorItem() override {
		
	}
	std::string GetName() override {
		return "Step_WaitAnimation";
	}
};

//指定時間で指定位置に移動する
class Step_MoveFixedPositionTime : public BaseStep {
private:
	Vector3 position_;
	float time_;
public:
	void Initialize(Vector3 position, float time) { position_ = position;  time_ = time; }
	void Activate(BossAction* action) override;

	nlohmann::json WriteStep() override {
		return{
			{"step","Step_MoveFixedPsitionTime"},
			{"position",
				{
					{"x",position_.x},
					{"y",position_.y},
					{"z",position_.z},
				}
			},
			{"time",time_},
		};
	}

	void ReadStep(const nlohmann::json_abi_v3_12_0::json& stepJson) override {
		Initialize(Vector3{ 
				stepJson["position"]["x"],
				stepJson["position"]["y"],
				stepJson["position"]["z"],
			}, 
			stepJson["time"]);
	}

	void EditorItem() override {
#ifdef USE_IMGUI
		ImGui::DragFloat3("指定位置", &position_.x);
		ImGui::DragFloat("指定時間", &time_);
#endif
	}
	std::string GetName() override {
		return "Step_MoveFixedPsitionTime";
	}
};
//指定速度で指定位置に移動する
class Step_MoveFixedPositionSpeed : public BaseStep {
private:
	float speed_;
	Vector3 position_;
public:
	void Initialize(float speed, Vector3 position) { speed_ = speed; position_ = position; }
	void Activate(BossAction* action) override;

	nlohmann::json WriteStep() override {
		return{
			{"step","Step_MoveFixedPsitionSpeed"},
			{"speed",speed_},
			{"position",
				{
					{"x",position_.x},
					{"y",position_.y},
					{"z",position_.z},
				}
			},
		};
	}

	void ReadStep(const nlohmann::json_abi_v3_12_0::json& stepJson) override {
		Initialize(stepJson["speed"],
			Vector3{
				stepJson["position"]["x"],
				stepJson["position"]["y"],
				stepJson["position"]["z"],
			});
	}

	void EditorItem() override {
#ifdef USE_IMGUI
		ImGui::DragFloat("指定速度", &speed_);
		ImGui::DragFloat3("指定位置", &position_.x);
#endif
	}
	std::string GetName() override {
		return "Step_MoveFixedPsitionSpeed";
	}
};
//指定速度で移動する
class Step_MoveFixedVelocity : public BaseStep {
private:
	Vector3 velocity_;
	float time_;
public:
	void Initialize(Vector3 velocity, float time) { velocity_ = velocity;  time_ = time; }
	void Activate(BossAction* action) override;

	nlohmann::json WriteStep() override {
		return{
			{"step","Step_MoveFixedVelocity"},
			{"velocity",
				{
					{"x",velocity_.x},
					{"y",velocity_.y},
					{"z",velocity_.z},
				}
			},
			{"time",time_},
		};
	}

	void ReadStep(const nlohmann::json_abi_v3_12_0::json& stepJson) override {
		Initialize(Vector3{
				stepJson["velocity"]["x"],
				stepJson["velocity"]["y"],
				stepJson["velocity"]["z"],
			},
			stepJson["time"]);
	}
	void EditorItem() override {
#ifdef USE_IMGUI
		ImGui::DragFloat3("指定速度", &velocity_.x);
		ImGui::DragFloat("指定時間", &time_);
#endif
	}
	std::string GetName() override {
		return "Step_MoveFixedVelocity";
	}
};
//向いてる方向に移動する
class Step_MoveFront : public BaseStep {
private:
	float speed_;
	float time_;
public:
	void Initialize(float speed, float time) { speed_ = speed;  time_ = time; }
	void Activate(BossAction* action) override;

	nlohmann::json WriteStep() override {
		return{
			{"step","Step_MoveFront"},
			{"speed",speed_},
			{"time",time_},
		};
	}

	void ReadStep(const nlohmann::json_abi_v3_12_0::json& stepJson) override {
		Initialize(stepJson["speed"], stepJson["time"]);
	}
	void EditorItem() override {
#ifdef USE_IMGUI
		ImGui::DragFloat("指定速度", &speed_);
		ImGui::DragFloat("指定時間", &time_);
#endif
	}
	std::string GetName() override {
		return "Step_MoveFront";
	}
};
//LockOn対象に向けて移動する
class Step_MoveToLockOn : public BaseStep {
private:
	float speed_;
	float time_;
public:
	void Initialize(float speed, float time) { speed_ = speed;  time_ = time; }
	void Activate(BossAction* action) override;

	nlohmann::json WriteStep() override {
		return{
			{"step","Step_MoveToLockOn"},
			{"speed",speed_},
			{"time",time_},
		};
	}

	void ReadStep(const nlohmann::json_abi_v3_12_0::json& stepJson) override {
		Initialize(stepJson["speed"], stepJson["time"]);
	}
	void EditorItem() override {
#ifdef USE_IMGUI
		ImGui::DragFloat("指定速度", &speed_);
		ImGui::DragFloat("指定時間", &time_);
#endif
	}
	std::string GetName() override {
		return "Step_MoveToLockOn";
	}
};

//プレイヤーをロックオンする
class Step_LockOnPlayer : public BaseStep {
private:

public:
	void Activate(BossAction* action) override;

	nlohmann::json WriteStep() override {
		return{
			{"step","Step_LockOnPlayer"},
		};
	}

	void ReadStep(const nlohmann::json_abi_v3_12_0::json& stepJson) override {

	}
	void EditorItem() override {

	}
	std::string GetName() override {
		return "Step_LockOnPlayer";
	}
};
//ロックオン解除
class Step_LockOnRelease : public BaseStep {
private:

public:
	void Activate(BossAction* action) override;

	nlohmann::json WriteStep() override {
		return{
			{"step","Step_LockOnRelease"},
		};
	}

	void ReadStep(const nlohmann::json_abi_v3_12_0::json& stepJson) override {

	}
	void EditorItem() override {

	}
	std::string GetName() override {
		return "Step_LockOnRelease";
	}
};

//向いてる方向に弾発射
class Step_ShotBulletToFront : public BaseStep {
private:
	float spread_;
	float speed_;
public:
	void Initialize(float spread, float speed) { spread_ = spread; speed_ = speed; }
	void Activate(BossAction* action) override;

	nlohmann::json WriteStep() override {
		return{
			{"step","Step_ShotBulletToFront"},
			{"spread",spread_},
			{"speed",speed_},
		};
	}

	void ReadStep(const nlohmann::json_abi_v3_12_0::json& stepJson) override {
		Initialize(stepJson["spread"], stepJson["speed"]);
	}
	void EditorItem() override {
#ifdef USE_IMGUI
		ImGui::DragFloat("拡散角度", &spread_);
		ImGui::DragFloat("指定速度", &speed_);
#endif
	}
	std::string GetName() override {
		return "Step_ShotBulletToFront";
	}
};
/*
//向いてる方向に弾発射
class Step_ShotBulletToBone : public BaseStep {
private:
	std::string boneName_;
	float spread_;
	float speed_;
#ifdef USE_IMGUI
	char inputBoneName_[64] = {};
#endif
public:
	void Initialize(std::string boneName, float spread, float speed) { boneName_ = boneName; spread_ = spread; speed_ = speed; }
	void Activate(BossAction* action) override;

	nlohmann::json WriteStep() override {
		return{
			{"step","Step_ShotBulletToBone"},
			{"boneName",boneName_},
			{"spread",spread_},
			{"speed",speed_},
		};
	}

	void ReadStep(const nlohmann::json_abi_v3_12_0::json& stepJson) override {
		Initialize(stepJson["boneName"], stepJson["spread"], stepJson["speed"]);
#ifdef USE_IMGUI
		strncpy_s(inputBoneName_, boneName_.c_str(), sizeof(inputBoneName_));
#endif
	}
	void EditorItem() override {
#ifdef USE_IMGUI

		if (ImGui::InputText("ボーン名", inputBoneName_, sizeof(inputBoneName_))) {
			boneName_ = inputBoneName_;
		}
		ImGui::DragFloat("拡散角度", &spread_);
		ImGui::DragFloat("指定速度", &speed_);
#endif
	}
	std::string GetName() override {
		return "Step_ShotBulletToBone";
	}
};

//爆弾投擲
class Step_ThrowBombToFixedPosition : public BaseStep {
private:
	Vector3 position_;
	float spread_;
	float speed_;
public:
	void Initialize(Vector3 position, float spread, float speed) { position_ = position; spread_ = spread; speed_ = speed; }
	void Activate(BossAction* action) override;

	nlohmann::json WriteStep() override {
		return{
			{"step","Step_ThrowBombToFixedPosition"},
			{"position",position_},
			{"spread",spread_},
			{"speed",speed_},
		};
	}

	void ReadStep(const nlohmann::json_abi_v3_12_0::json& stepJson) override {
		Initialize(stepJson["position"].get<Vector3>(), stepJson["spread"], stepJson["speed"]);
	}
	void EditorItem() override {
#ifdef USE_IMGUI
		ImGui::DragFloat3("指定位置", &position_.x);
		ImGui::DragFloat("拡散角度", &spread_);
		ImGui::DragFloat("指定速度", &speed_);
#endif
	}
	std::string GetName() override {
		return "Step_ThrowBombToFixedPosition";
	}
};

//爆弾投擲
class Step_ThrowBombToBone : public BaseStep {
private:
	std::string boneName_;
	float spread_;
	float speed_;
#ifdef USE_IMGUI
	char inputBoneName_[64] = {};
#endif
public:
	void Initialize(std::string boneName, float spread, float speed) { boneName_ = boneName; spread_ = spread; speed_ = speed; }
	void Activate(BossAction* action) override;

	nlohmann::json WriteStep() override {
		return{
			{"step","Step_ShotBulletToBone"},
			{"boneName",boneName_},
			{"spread",spread_},
			{"speed",speed_},
		};
	}

	void ReadStep(const nlohmann::json_abi_v3_12_0::json& stepJson) override {
		Initialize(stepJson["boneName"], stepJson["spread"], stepJson["speed"]);
#ifdef USE_IMGUI
		strncpy_s(inputBoneName_, boneName_.c_str(), sizeof(inputBoneName_));
#endif
	}
	void EditorItem() override {
#ifdef USE_IMGUI

		if (ImGui::InputText("ボーン名", inputBoneName_, sizeof(inputBoneName_))) {
			boneName_ = inputBoneName_;
		}
		ImGui::DragFloat("拡散角度", &spread_);
		ImGui::DragFloat("指定速度", &speed_);
#endif
	}
	std::string GetName() override {
		return "Step_ShotBulletToBone";
	}
};*/

#pragma endregion

std::unique_ptr<BaseStep> ReadStepJson(const nlohmann::json_abi_v3_12_0::json& stepJson);

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
	//特定の部位が存在している場合
	//std::string partsName;

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

class Stage;

class Boss : public BaseCharacter
{
public:
	//ボス状態
	enum class State {
		None,				//無し。初期状態
		Event,				//イベント。ボスがアニメーションする
		Move,				//移動。行動が使えなかったりするなら行う
		PatternExecution	//行動
	};

private:

	//ボス状態
	State state_ = State::None;

	float angle;

	//ターゲット用Sphere
	std::shared_ptr<Sphere> trackingSphere_;

	//体力
	float maxHP_;
	float HP_;

	std::shared_ptr<GameCamera> gameCamera_ = nullptr;

	std::unique_ptr<ParticleEmitter> moveParticle_ = nullptr;

	Stage* stage_ = nullptr;
	Player* player_ = nullptr;

	//ボスの行動map
	std::unordered_map<std::string, std::unique_ptr<BossPattern>> patterns_;
	//map用パターン名
	std::string patternName_;

	//線形補完位置
	std::optional<LerpPositionState> lerpPosition_{};
	float lerpPositionTime_;
	//線形補完移動
	std::optional<VelocityState> velocityState_{};
	float velocityStateTime_;

	bool isStartAnimation_ = false;

	//ロックオンする場合の相手の座標
	Vector3 lockOnPosition_;

	bool isAction_;

public:

	~Boss();

	//初期化
	void Initialize(std::string filepath, Stage* stage_, std::shared_ptr<GameCamera> gameCamera, Player* player, SRT startTransform);
	//更新
	void Update();
	//描画
	void Draw();

	void IsCollision(uint8_t targetId) override;

	void IsCollisionGround(OBB obb) override;

	SRT GetTransform() { return  transform_; }
	SRT* GetPlayerTransform() { return player_->GetTransform(); }
	Vector3 GetVelocity() { return velocity_; }
	Stage* GetStage() { return stage_; }
	Player* GetPlayer() { return player_; }
	std::shared_ptr<Sphere> GetTrackingSphere() { return trackingSphere_; }
	std::shared_ptr<GameCamera> GetGameCamera() { return gameCamera_; }

	void SetTransfrom(SRT transfrom) { transform_ = transfrom; object_->SetTransform(transform_); }

	State GetBossState() { return state_ ; }
	void SetBossState(State state) { state_ = state; }

	void SetCamera(shared_ptr<Camera> camera) { object_->SetCamera(camera); }

	void SetModel(std::shared_ptr<Model> model) { object_->Initialize(model); }

	void SetPatterns(std::string name, std::unique_ptr<BossPattern> pattern) { patterns_[name] = move(pattern); }

	void SetDirectionalLight(shared_ptr<DirectionalLight> directionalLight) { object_->SetDirectionalLight(directionalLight); }
	void SetPointLight(shared_ptr<PointLight> pointLight) { object_->SetPointLight(pointLight); }

	void Damage(float damage) { HP_ -= damage; }

	bool IsDead() { return HP_ <= 0; }

	bool IsStartAnimation() { return isStartAnimation_; }

	bool IsAction() { return isAction_; }

	void ShotBullet(Vector3 startPoint, Vector3 rotate, float speed);

	void EmitMoveParticle(SRT transform) { moveParticle_->Emit(transform); }

private:

	void ReadBossFile(std::string filePath);

	//次のパターン
	bool NextPattern();
};

