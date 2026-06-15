#include "Boss.h"
#include "GameEngine.h"
#include <Vector3.h>
#include <Matrix4x4.h>
#include <numbers>
#include <Math/Easing.h>
#include <GameManager/BaseScene/GameScene/GameScene.h>
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
	state.velocity = Vector3{ 0,0,speed_ } * MakeRotateXMatrix(action->GetBoss()->GetTransform()->rotate.x) * MakeRotateXMatrix(action->GetBoss()->GetTransform()->rotate.y);
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

void Step_ShotBullet::Activate(BossAction* action) {
	action->GetBoss()->ShotBullet(startPoint_, rotate_, speed_);
};

#pragma endregion

void BossAction::Update() {
	while (GetIsStop() && stepIndex_ < steps_.size())
	{
		steps_[stepIndex_]->Activate(this);
		stepIndex_++;
	}

	if (waitTime_ > 0.0f) {
		waitTime_ -= 1.0f / 60.0f;
	}

	if (isWaitStep_ && boss_->IsAction();) {
		isWaitStep_ = false;
	}
}

void BossPattern::Update() {
	action_->Update();
}

Boss::~Boss() {

}

void Boss::Initialize(GameScene* gameScene, GameCamera* gameCamera, ParticleEmitter* particle, Player* player, float maxHP) {
	maxHP_ = maxHP;
	HP_ = maxHP_;

	gameScene_ = gameScene;
	gameCamera_ = gameCamera;
	particle_ = particle;
	player_ = player;

	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Boss));
	object_->SetIsUseAnimation(true);
	object_->SetAnimationName("Start");
	object_->SetAnimationInterpolation(AnimationInterpolation::Cubic_Spline);
	transform_.scale = { 0.562558f * 2,0.562558f * 2,0.562558f * 2 };
	transform_.rotate = { 0.0f,0.0f,0.0f };
	transform_.translate = { 0.0f,1.0f,0.0f };
	object_->SetTransform(transform_);

	isStartAnimation_ = true;

	targetTransform_ = std::make_unique<SRT>();
	*targetTransform_ = transform_;

	//action_ = std::make_unique<BossAction_Shot_01>();
	//action_->Initialize(this);

	std::vector<std::unique_ptr<BaseStep>> steps;
	std::unique_ptr<BaseStep> step;
	steps.push_back(make_unique<Step_LockOnPlayer>());
	steps.push_back(make_unique<Step_WaitTime>(1.0f));

	std::unique_ptr<BossAction> action = std::make_unique<BossAction>();
	action->SetSteps(move(steps));

	PatternCondition condition{};
	condition.farDistance = 50.0f;
	condition.priority = 5;

	std::unique_ptr<BossPattern> pattern = std::make_unique<BossPattern>();
	pattern->Initialize(this);
	pattern->SetAction(move(action));
	pattern->SetCondition(condition);
	patterns_.push_back(move(pattern));

	BaseCharacter::Initialize(2.0f, CollisionID_Enemy_Body);
}

void Boss::Update() {

	if (isStartAnimation_) {
		if (object_->IsEndAnimation()) {
			object_->SetAnimationName("Start");
			object_->ResetAnimationTime();
			object_->SetIsLoopAnimation(true);
			isStartAnimation_ = false;
		}
	} else {

		if (HP_ > 0.0f) {

			Vector4 color;
			color = { (HP_ / maxHP_), (HP_ / maxHP_), (HP_ / maxHP_), 1.0f };
			object_->SetColor(color);

			/*if (HP_ / maxHP_ < 0.2f) {
				weights_[2] = 8;
				weights_[3] = 3;
			} else if (HP_ / maxHP_ < 0.5f) {
				weights_[2] = 4;
				weights_[3] = 6;
			} else {
				weights_[2] = 3;
				weights_[3] = 3;
			}

			//action_->Update();
			if (action_->IsEnd()) {
				action_->Finalize();
				action_.reset();

				int maxWeight = 0;
				for (int& weight : weights_) {
					maxWeight += weight;
				}
				int randInt = GameEngine::randomInt(1, maxWeight);
				if (randInt <= weights_[0]) {
					action_ = std::make_unique<BossAction_Shot_01>();
				} else if (randInt <= weights_[0] + weights_[1]) {
					action_ = std::make_unique<BossAction_Shot_03>();
				} else if (randInt <= weights_[0] + weights_[1] + weights_[2]) {
					action_ = std::make_unique<BossAction_Jump>();
				} else if (randInt <= weights_[0] + weights_[1] + weights_[2] + weights_[3]) {
					action_ = std::make_unique<BossAction_Move>();
				}

				action_->Initialize(this);
			}*/
			transform_ = *targetTransform_;
		}
	}
	SRT displayTransform = transform_;
	displayTransform.translate.y -= 1.0f;
	object_->SetTransform(displayTransform);
	object_->Update();
	BaseCharacter::Update();
}

void Boss::Draw() {
	object_->Draw3D();
}

void Boss::IsCollision(uint8_t targetId) {
	if (targetId & CollisionID_Player_Attack) {	//プレイヤー攻撃
		HP_--;
		particle_->Emit();
		gameCamera_->SetShakeTime(0.3f);
		gameScene_->SetRingColorA(1.0f);
	}
}

void Boss::IsCollisionGround(OBB obb) {

}

void Boss::ShotBullet(Vector3 startPoint, Vector3 rotate, float speed) {
	gameScene_->AddBossBullet(startPoint, rotate);
}