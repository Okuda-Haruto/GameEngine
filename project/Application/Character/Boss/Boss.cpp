#include "Boss.h"
#include "GameEngine.h"
#include <Vector3.h>
#include <Matrix4x4.h>
#include <numbers>
#include <Math/Easing.h>
#include <StageManager/Stage/Stage.h>
#include "Operation/Operation.h"

#pragma region Step

void Step_WaitTime::Activate(BossAction* action) {
	action->SetWaitTime(time_);
}

void Step_WaitStep::Activate(BossAction* action) {
	action->SetWaitStep(true);
}

void Step_WaitAnimation::Activate(BossAction* action) {
	action->SetWaitAnimation(true);
}

void Step_MoveFixedPsitionTime::Activate(BossAction* action) {
	LerpPositionState state;
	state.startVector = action->GetBoss()->GetTransform()->translate;
	state.endVector = position_;
	state.time = time_;
	state.type = 0;
	action->SetLerpPosition(state);
}

void Step_MoveFixedPsitionSpeed::Activate(BossAction* action) {
	//距離
	Vector3 diff = position_ - action->GetBoss()->GetTransform()->translate;
	//その距離移動するににかかる時間
	float lerpTime = Length(diff) / speed_;

	LerpPositionState state;
	state.startVector = action->GetBoss()->GetTransform()->translate;
	state.endVector = position_;
	state.time = lerpTime;
	state.type = 0;
	action->SetLerpPosition(state);
}

void Step_MoveFixedVelocity::Activate(BossAction* action) {
	VelocityState state;
	state.velocity = velocity_;
	state.time = time_;
	action->SetVelocityState(state);
}

void Step_MoveFront::Activate(BossAction* action) {
	VelocityState state;
	state.velocity = Vector3{ 0,0,speed_ } * MakeRotateXMatrix(action->GetBoss()->GetTransform()->rotate.x) * MakeRotateYMatrix(action->GetBoss()->GetTransform()->rotate.y);
	state.time = time_;
	action->SetVelocityState(state);
}

void Step_MoveToLockOn::Activate(BossAction* action) {
	Vector3 diff = Normalize(action->GetBoss()->GetPlayerTransform()->translate - action->GetBoss()->GetTransform()->translate);

	VelocityState state;
	state.velocity = diff * speed_;
	state.time = time_;
	action->SetVelocityState(state);
}

void Step_LockOnPlayer::Activate(BossAction* action) {
	action->SetIsLockOnPlayer(true);
}

void Step_LockOnRelease::Activate(BossAction* action) {
	action->SetIsLockOnPlayer(false);
}

void Step_ShotBulletToFront::Activate(BossAction* action) {
	Vector3 bulletSpread{};
	if (spread_ > 0.0f) {
		bulletSpread = Vector3{ 0,GameEngine::randomFloat(0.0f,spread_),0 } * MakeRotateZMatrix(GameEngine::randomFloat(0.0f,std::numbers::pi_v<float> * 2));
	}
	action->GetBoss()->ShotBullet(
		action->GetBoss()->GetTransform()->translate,
		action->GetBoss()->GetTransform()->rotate + bulletSpread,
		speed_
	);
};

#pragma endregion

std::unique_ptr<BaseStep> ReadStepJson(const nlohmann::json_abi_v3_12_0::json& stepJson) {

	using StepCreator = std::function<std::unique_ptr<BaseStep>()>;

	static const std::unordered_map<std::string, StepCreator> creators =
	{
		{ "Step_WaitTime",   [] { return std::make_unique<Step_WaitTime>(); }},
		{ "Step_WaitStep", [] { return std::make_unique<Step_WaitStep>(); } },
		{ "Step_WaitAnimation",   [] { return std::make_unique<Step_WaitAnimation>(); } },
		{ "Step_MoveFixedPsitionTime",   [] { return std::make_unique<Step_MoveFixedPsitionTime>(); } },
		{ "Step_MoveFixedPsitionSpeed",   [] { return std::make_unique<Step_MoveFixedPsitionSpeed>(); } },
		{ "Step_MoveFixedVelocity",   [] { return std::make_unique<Step_MoveFixedVelocity>(); } },
		{ "Step_MoveFront",   [] { return std::make_unique<Step_MoveFront>(); } },
		{ "Step_MoveToLockOn",   [] { return std::make_unique<Step_MoveToLockOn>(); } },
		{ "Step_LockOnPlayer",   [] { return std::make_unique<Step_LockOnPlayer>(); } },
		{ "Step_LockOnRelease",   [] { return std::make_unique<Step_LockOnRelease>(); } },
		{ "Step_ShotBulletToFront",   [] { return std::make_unique<Step_ShotBulletToFront>(); } },
	};

	auto it = creators.find(stepJson["step"]);

	if (it == creators.end()) {
		return nullptr;
	}

	std::unique_ptr<BaseStep> step = it->second();
	step->ReadStep(stepJson);

	return step;
}

void BossAction::Update() {
	if (!isEnd_) {

		if (stepIndex_ == steps_.size()) {
			isEnd_ = true;
		}

		if (waitTime_ > 0.0f) {
			waitTime_ -= 1.0f / 60.0f;
		}

		if (isWaitStep_ && !boss_->IsAction()) {
			isWaitStep_ = false;
		}

		while (!GetIsStop() && stepIndex_ < steps_.size())
		{
			steps_[stepIndex_]->Activate(this);
			stepIndex_++;
		}
	}
}

void BossPattern::Update() {
	action_->Update();

	isEnd_ = action_->IsEnd();
}

Boss::~Boss() {

}

void Boss::Initialize(std::string filepath, Stage* stage, std::shared_ptr<GameCamera> gameCamera, Player* player, SRT startTransform) {

	//ReadBossFile(filepath);
	
	maxHP_ = 50;
	HP_ = 50;

	stage_ = stage;
	gameCamera_ = gameCamera;
	player_ = player;

	//モデルの生成		とりあえずプレイヤーと同じ
	object_ = std::make_unique<Object>();
	object_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Player));
	//object_->SetIsUseAnimation(true);
	//object_->SetAnimationName("Start");
	object_->SetAnimationInterpolation(AnimationInterpolation::Cubic_Spline);
	transform_ = startTransform;
	object_->SetTransform(transform_);

	isStartAnimation_ = true;

	targetTransform_ = std::make_unique<SRT>();
	*targetTransform_ = transform_;

	std::unique_ptr<Step_WaitTime> step_WaitTime;
	std::unique_ptr<Step_ShotBulletToFront> step_ShotBulletToFront;
	std::unique_ptr<Step_MoveFront> step_MoveFront;
	//1
	std::vector<std::unique_ptr<BaseStep>> steps;
	steps.push_back(make_unique<Step_LockOnPlayer>());
	step_WaitTime = std::make_unique<Step_WaitTime>();
	step_WaitTime->Initialize(0.5f);
	steps.push_back(move(step_WaitTime));
	steps.push_back(make_unique<Step_LockOnRelease>());
	step_WaitTime = std::make_unique<Step_WaitTime>();
	step_WaitTime->Initialize(0.25f);
	steps.push_back(move(step_WaitTime));
	step_ShotBulletToFront = make_unique<Step_ShotBulletToFront>();
	step_ShotBulletToFront->Initialize(0.05f, 1.0f);
	steps.push_back(move(step_ShotBulletToFront));
	steps.push_back(make_unique<Step_LockOnPlayer>());
	step_WaitTime = std::make_unique<Step_WaitTime>();
	step_WaitTime->Initialize(0.5f);
	steps.push_back(move(step_WaitTime));
	steps.push_back(make_unique<Step_LockOnRelease>());
	step_WaitTime = std::make_unique<Step_WaitTime>();
	step_WaitTime->Initialize(0.25f);
	steps.push_back(move(step_WaitTime));
	step_ShotBulletToFront = make_unique<Step_ShotBulletToFront>();
	step_ShotBulletToFront->Initialize(0.05f, 1.0f);
	steps.push_back(move(step_ShotBulletToFront));
	steps.push_back(make_unique<Step_LockOnPlayer>());
	step_WaitTime = std::make_unique<Step_WaitTime>();
	step_WaitTime->Initialize(0.75f);
	steps.push_back(move(step_WaitTime));
	steps.push_back(make_unique<Step_LockOnRelease>());
	step_WaitTime = std::make_unique<Step_WaitTime>();
	step_WaitTime->Initialize(0.25f);
	steps.push_back(move(step_WaitTime));
	for (int i = 0; i < 10; i++) {
		step_ShotBulletToFront = make_unique<Step_ShotBulletToFront>();
		step_ShotBulletToFront->Initialize(0.05f, 1.0f);
		steps.push_back(move(step_ShotBulletToFront));
		step_WaitTime = std::make_unique<Step_WaitTime>();
		step_WaitTime->Initialize(0.01f);
		steps.push_back(move(step_WaitTime));
	}
	step_WaitTime = std::make_unique<Step_WaitTime>();
	step_WaitTime->Initialize(1.0f);
	steps.push_back(move(step_WaitTime));



	std::unique_ptr<BossAction> action = std::make_unique<BossAction>();
	action->SetSteps(move(steps));

	PatternCondition condition{};
	condition.farDistance = 15.0f;
	condition.priority = 5;

	std::unique_ptr<BossPattern> pattern = std::make_unique<BossPattern>();
	pattern->SetAction(move(action));
	pattern->SetCondition(condition);
	pattern->Initialize(this);
	patterns_["3shot"] = move(pattern);

	//2
	steps.clear();
	steps.push_back(make_unique<Step_LockOnPlayer>());
	step_WaitTime = std::make_unique<Step_WaitTime>();
	step_WaitTime->Initialize(2.0f);
	steps.push_back(move(step_WaitTime));
	steps.push_back(make_unique<Step_LockOnRelease>());
	step_MoveFront = std::make_unique<Step_MoveFront>();
	step_MoveFront->Initialize(0.8f, 0.5f);
	steps.push_back(move(step_MoveFront));
	steps.push_back(make_unique<Step_WaitStep>());

	action.reset();
	action = std::make_unique<BossAction>();
	action->SetSteps(move(steps));

	condition = {};
	condition.nearDistance = 30.0f;
	condition.priority = 5;

	pattern.reset();
	pattern = std::make_unique<BossPattern>();
	pattern->SetAction(move(action));
	pattern->SetCondition(condition);
	pattern->Initialize(this);
	patterns_["FrontMove"] = move(pattern);

	//3
	steps.clear();
	steps.push_back(make_unique<Step_LockOnPlayer>());
	step_WaitTime = std::make_unique<Step_WaitTime>();
	step_WaitTime->Initialize(0.2f);
	steps.push_back(move(step_WaitTime));
	step_MoveFront = std::make_unique<Step_MoveFront>();
	step_MoveFront->Initialize(0.4f, 1.0f);
	steps.push_back(move(step_MoveFront));
	steps.push_back(make_unique<Step_WaitStep>());
	steps.push_back(make_unique<Step_LockOnRelease>());

	action.reset();
	action = std::make_unique<BossAction>();
	action->SetSteps(move(steps));

	condition = {};
	condition.farDistance = 50.0f;
	condition.priority = 8;

	pattern.reset();
	pattern = std::make_unique<BossPattern>();
	pattern->SetAction(move(action));
	pattern->SetCondition(condition);
	pattern->Initialize(this);
	patterns_["FarPlayer_01"] = move(pattern);

	//4
	steps.clear();
	steps.push_back(make_unique<Step_LockOnPlayer>());
	step_WaitTime = std::make_unique<Step_WaitTime>();
	step_WaitTime->Initialize(1.0f);
	steps.push_back(move(step_WaitTime));
	steps.push_back(make_unique<Step_LockOnRelease>());
	step_MoveFront = std::make_unique<Step_MoveFront>();
	step_MoveFront->Initialize(1.8f, 0.75f);
	steps.push_back(move(step_MoveFront));
	steps.push_back(make_unique<Step_WaitStep>());
	steps.push_back(make_unique<Step_LockOnPlayer>());
	step_WaitTime = std::make_unique<Step_WaitTime>();
	step_WaitTime->Initialize(0.5f);
	steps.push_back(move(step_WaitTime));
	steps.push_back(make_unique<Step_LockOnRelease>());
	step_MoveFront = std::make_unique<Step_MoveFront>();
	step_MoveFront->Initialize(1.8f, 0.75f);
	steps.push_back(move(step_MoveFront));
	steps.push_back(make_unique<Step_WaitStep>());
	steps.push_back(make_unique<Step_LockOnPlayer>());
	step_WaitTime = std::make_unique<Step_WaitTime>();
	step_WaitTime->Initialize(0.5f);
	steps.push_back(move(step_WaitTime));
	steps.push_back(make_unique<Step_LockOnRelease>());
	step_MoveFront = std::make_unique<Step_MoveFront>();
	step_MoveFront->Initialize(1.8f, 0.75f);
	steps.push_back(move(step_MoveFront));
	steps.push_back(make_unique<Step_WaitStep>());
	step_WaitTime = std::make_unique<Step_WaitTime>();
	step_WaitTime->Initialize(0.5f);
	steps.push_back(move(step_WaitTime));

	action.reset();
	action = std::make_unique<BossAction>();
	action->SetSteps(move(steps));

	condition = {};
	condition.maxHpRate = 0.5f;
	condition.priority = 6;

	pattern.reset();
	pattern = std::make_unique<BossPattern>();
	pattern->SetAction(move(action));
	pattern->SetCondition(condition);
	pattern->Initialize(this);
	patterns_["Dash"] = move(pattern);

	//5
	steps.clear();
	steps.push_back(make_unique<Step_LockOnPlayer>());
	step_WaitTime = std::make_unique<Step_WaitTime>();
	step_WaitTime->Initialize(0.2f);
	steps.push_back(move(step_WaitTime));
	step_MoveFront = std::make_unique<Step_MoveFront>();
	step_MoveFront->Initialize(0.6f, 0.8f);
	steps.push_back(move(step_MoveFront));
	steps.push_back(make_unique<Step_WaitStep>());
	step_WaitTime = std::make_unique<Step_WaitTime>();
	step_WaitTime->Initialize(0.2f);
	steps.push_back(move(step_WaitTime));
	steps.push_back(make_unique<Step_LockOnRelease>());
	step_ShotBulletToFront = make_unique<Step_ShotBulletToFront>();
	step_ShotBulletToFront->Initialize(0.05f, 1.0f);
	steps.push_back(move(step_ShotBulletToFront));

	action.reset();
	action = std::make_unique<BossAction>();
	action->SetSteps(move(steps));

	condition = {};
	condition.farDistance = 40.0f;
	condition.maxHpRate = 0.5f;
	condition.priority = 9;

	pattern.reset();
	pattern = std::make_unique<BossPattern>();
	pattern->SetAction(move(action));
	pattern->SetCondition(condition);
	pattern->Initialize(this);
	patterns_["FarPlayer_02"] = move(pattern);

	patternName_ = {};
	lerpPositionTime_ = 0;
	velocityStateTime_ = 0;

	BaseCharacter::Initialize(1.5f, CollisionID_Enemy_Body);
}

void Boss::Update() {

	if (isStartAnimation_) {
		if (object_->IsEndAnimation()) {
			object_->SetAnimationName("Idle");
			object_->ResetAnimationTime();
			object_->SetIsLoopAnimation(true);
			isStartAnimation_ = false;
		}
	} else {

		if (HP_ > 0.0f) {

			Vector4 color;
			color = { (HP_ / maxHP_) / 2, (HP_ / maxHP_) / 2, (HP_ / maxHP_), 1.0f };
			object_->SetColor(color);

			if (!patternName_.empty()) {
				patterns_[patternName_]->Update();
				velocity_ = {};
				BossAction* action = patterns_[patternName_]->GetAction();
				isAction_ = false;
				if (!action->IsEnd()) {
					//位置線形補完
					lerpPosition_ = action->GetLerpPosition();
					if (lerpPosition_) {
						transform_.translate = Lerp(lerpPosition_->startVector, lerpPosition_->endVector, lerpPositionTime_);
						lerpPositionTime_ += 1.0f / 60.0f;
						if (lerpPositionTime_ > lerpPosition_->time) {
							action->ResetLerpPosition();
							lerpPositionTime_ = 0.0f;
						}
						isAction_ = true;
					}

					//速度
					velocityState_ = action->GetVelocityState();
					if (velocityState_) {
						velocity_ = velocityState_->velocity;
						velocityStateTime_ += 1.0f / 60.0f;
						if (velocityStateTime_ > velocityState_->time) {
							action->ResetVelocityState();
							velocityStateTime_ = 0.0f;
						}
						isAction_ = true;
					}

					//ロックオン中か
					if (action->GetIsLockOnPlayer()) {
						SRT playerTransform = *player_->GetTransform();
						lockOnPosition_ = playerTransform.translate;

						//プレイヤー方向を向かせる
						Vector3 diff = Normalize(Vector3(playerTransform.translate.x, 0.0f, playerTransform.translate.z) - Vector3(transform_.translate.x, 0.0f, transform_.translate.z));
						float angle = std::atan2(diff.x, diff.z);

						//最短角度補完
						float d = angle - transform_.rotate.y;

						if (d >= std::numbers::pi_v<float>*2) {
							d = angle - transform_.rotate.y;
						}

						d = std::fmodf(d, std::numbers::pi_v<float> *2);

						if (d > std::numbers::pi_v<float>) {
							d -= std::numbers::pi_v<float> *2;
						} else if (d < -std::numbers::pi_v<float>) {
							d += std::numbers::pi_v<float> *2;
						}

						if (d > std::numbers::pi_v<float>) {
							d -= std::numbers::pi_v<float> *2;
						} else if (d < -std::numbers::pi_v<float>) {
							d += std::numbers::pi_v<float> *2;
						}

						//正直あまりやりたくはない方法だが2πを超えると遠回りで回転してしまうので致し方無い
						transform_.rotate.y = (transform_.rotate.y + d * 0.2f);
						transform_.rotate.y = std::fmodf(transform_.rotate.y, std::numbers::pi_v<float> *2);
					}
				} else {
					NextPattern();
				}
			} else {
				NextPattern();
			}

			transform_.translate += velocity_;
			transform_.translate.y = 1.0f;

			transform_.translate.x = std::clamp(transform_.translate.x, -59.0f, 59.0f);
			transform_.translate.z = std::clamp(transform_.translate.z, -59.0f, 59.0f);

			*targetTransform_ = transform_;
		}
	}
	SRT displayTransform = transform_;
	displayTransform.translate.y = 1.5f;
	object_->SetTransform(displayTransform);
	object_->Update();
	BaseCharacter::Update();
}

void Boss::Draw() {
	object_->Draw3D();
}

void Boss::IsCollision(uint8_t targetId) {
	if (targetId == CollisionID_Player_Attack) {	//プレイヤー攻撃
		HP_--;
		//particle_->Emit();
		gameCamera_->SetShakeTime(0.3f);
	}
}

void Boss::IsCollisionGround(OBB obb) {

}

void Boss::ShotBullet(Vector3 startPoint, Vector3 rotate, float speed) {
	stage_->AddBossBullet(startPoint, rotate);
}

void Boss::ReadBossFile(std::string filePath) {

	//読み込むJsonファイル
	std::ifstream file(filePath.c_str());
	if (!file.is_open()) {
		//エラー処理
		object_ = std::make_unique<Object>();
		object_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/sphere","sphere.obj"));
		maxHP_ = 1;
		HP_ = maxHP_;

		return;
	}

	nlohmann::json bossJson;
	file >> bossJson;
	file.close();

	//基本ステータス
	bossName_ = bossJson["name"];

	object_ = std::make_unique<Object>();
	object_->Initialize(ModelManager::GetInstance()->GetModel(bossJson["name"]["directoryPath"], bossJson["name"]["modelname"]));

	maxHP_ = bossJson["name"]["maxHP"];
	HP_ = maxHP_;



	nlohmann::json& patternJson = bossJson["name"]["pattern"];

	//読み込んだパターン
	for (auto iterator = patternJson.begin(); iterator != patternJson.end(); ++iterator) {
		const std::string& name = iterator.key();
		const auto& stepArray = iterator.value();

		auto pattern = std::make_unique<BossPattern>();
		auto action = std::make_unique<BossAction>();

		//Actionを複数にするときに変える

		//ステップ読み込み
		std::vector<std::unique_ptr<BaseStep>> steps;
		for (const auto& stepJson : stepArray)
		{
			steps.push_back(ReadStepJson(stepJson["name"]["pattern"][name]));
		}
		action->SetSteps(move(steps));

		pattern->SetAction(std::move(action));

		patterns_.emplace(name, std::move(pattern));
	}
}

void Boss::NextPattern() {
	int8_t maxPriority = 0;

	float distance = Length(player_->GetTransform()->translate - transform_.translate);
	std::vector<std::string> patternNames;
	for (auto& pattern : patterns_) {
		PatternCondition condition = pattern.second->GetCondition();

		//優先度が同じ場合も通す
		if (condition.priority >= maxPriority) {
			//近距離判定
			if (condition.nearDistance && condition.nearDistance < distance) {
				continue;
			}
			//遠距離判定
			if (condition.farDistance && condition.farDistance > distance) {
				continue;
			}
			//少HP判定
			if (condition.minHpRate && condition.minHpRate > HP_ / maxHP_) {
				continue;
			}
			//多HP判定
			if (condition.maxHpRate && condition.maxHpRate < HP_ / maxHP_) {
				continue;
			}

			//優先度がより高い場合それまでの候補を消す
			if (condition.priority > maxPriority) {
				maxPriority = condition.priority;
				patternNames.clear();
			}
			//どれにも該当しないなら候補に加える
			patternNames.push_back(pattern.first);
		}
	}

	//空白のまま送る
	if (patternNames.empty()) {
		patternName_ = {};
		return;
	} else if (patternNames.size() == 1) {
		patternName_ = patternNames[0];
	} else {
		patternName_ = patternNames[GameEngine::randomInt(0, int(patternNames.size()) - 1)];
	}

	patterns_[patternName_]->Initialize(this);
}