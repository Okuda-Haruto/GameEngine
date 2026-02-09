#include "Boss.h"
#include "GameEngine.h"
#include <Vector3.h>
#include <Matrix4x4.h>
#include <numbers>
#include "Math/Lerp.h"
#include "Scene/GameScene/GameScene.h"
#include "Operation/Operation.h"

#pragma region BossAction_Shot_01

void BossAction_Shot_01::Initialize(Boss* boss) {
	boss_ = boss;

	actionTime_ = 0.0f;
	shotCooltime_ = 0.0f;
	targetedCooltime_ = 0.0f;

	isTargeted_ = false;
	isEnd_ = false;
}

void BossAction_Shot_01::Update() {
	actionTime_ += 1.0f / 60.0f;
	if (actionTime_ > kMaxActionTime_) {
		isEnd_ = true;
	}

	if (isTargeted_) {	//狙って撃つ
		shotCooltime_ += 1.0f / 60.0f;
		if (shotCooltime_ >= kMaxShotCooltime_) {
			SRT* BossTransform = boss_->GetTransform();
			Vector3 rotate = BossTransform->rotate;
			rotate.x += GameEngine::randomFloat(-0.01f, 0.01f);
			rotate.y += GameEngine::randomFloat(-0.06f, 0.06f);

			boss_->GetGameScene()->AddBossBullet(BossTransform->translate, rotate);
			shotCooltime_ -= kMaxShotCooltime_;
			isTargeted_ = false;

			boss_->GetGameCamera()->SetShakeTime(0.1f);
		}
	} else {			//プレイヤーを狙う

		//プレイヤー方向を向かせる
		SRT* PlayerTransform = boss_->GetPlayer()->GetTransform();
		SRT* BossTransform = boss_->GetTransform();
		Vector3 diff = Normalize(Vector3(PlayerTransform->translate.x, 0.0f, PlayerTransform->translate.z) - Vector3(BossTransform->translate.x, 0.0f, BossTransform->translate.z));
		float angle = std::atan2(diff.x, diff.z);

		//最短角度補完
		float d = angle - BossTransform->rotate.y;

		if (d >= std::numbers::pi_v<float>*2) {
			d = angle - BossTransform->rotate.y;
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
		BossTransform->rotate.y = (BossTransform->rotate.y + d * 0.2f);
		BossTransform->rotate.y = std::fmodf(BossTransform->rotate.y, std::numbers::pi_v<float> *2);

		//開始時少しだけ何もしない
		if (actionTime_ >= 0.5f) {
			targetedCooltime_ += 1.0f / 60.0f;
			if (targetedCooltime_ >= kMaxTargetedCooltime_) {
				targetedCooltime_ -= kMaxTargetedCooltime_;
				isTargeted_ = true;
				AudioHolder::GetInstance()->GetAudio(AudioIndex::Reload_SE).lock()->SoundPlayWave();
			}
		}
	}
}

void BossAction_Shot_01::Finalize() {

}

#pragma endregion

#pragma region BossAction_Shot_02

void BossAction_Shot_02::Initialize(Boss* boss) {
	boss_ = boss;

	actionTime_ = 0.0f;
	shotCooltime_ = 0.0f;
	targetedCooltime_ = 0.0f;

	isTargeted_ = false;
	isEnd_ = false;
}

void BossAction_Shot_02::Update() {
	actionTime_ += 1.0f / 60.0f;
	if (actionTime_ > kMaxActionTime_) {
		isEnd_ = true;
	}

	if (isTargeted_) {	//狙って撃つ
		shotCooltime_ += 1.0f / 60.0f;
		if (shotCooltime_ >= kMaxShotCooltime_) {
			SRT* BossTransform = boss_->GetTransform();
			Vector3 rotate = BossTransform->rotate;
			rotate.x += GameEngine::randomFloat(-0.01f, 0.01f);
			rotate.y += GameEngine::randomFloat(-0.12f, 0.12f);

			boss_->GetGameScene()->AddBossBullet(BossTransform->translate, rotate);
			shotCooltime_ -= kMaxShotCooltime_;

			boss_->GetGameCamera()->SetShakeTime(0.1f);
		}
	} else {			//プレイヤーを狙う

		//プレイヤー方向を向かせる
		SRT* PlayerTransform = boss_->GetPlayer()->GetTransform();
		SRT* BossTransform = boss_->GetTransform();
		Vector3 diff = Normalize(Vector3(PlayerTransform->translate.x, 0.0f, PlayerTransform->translate.z) - Vector3(BossTransform->translate.x, 0.0f, BossTransform->translate.z));
		float angle = std::atan2(diff.x, diff.z);

		//最短角度補完
		float d = angle - BossTransform->rotate.y;

		if (d >= std::numbers::pi_v<float>*2) {
			d = angle - BossTransform->rotate.y;
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
		BossTransform->rotate.y = (BossTransform->rotate.y + d * 0.2f);
		BossTransform->rotate.y = std::fmodf(BossTransform->rotate.y, std::numbers::pi_v<float> *2);

		//開始時少しだけ何もしない
		if (actionTime_ >= 0.5f) {
			targetedCooltime_ += 1.0f / 60.0f;
			if (targetedCooltime_ >= kMaxTargetedCooltime_) {
				targetedCooltime_ -= kMaxTargetedCooltime_;
				isTargeted_ = true;
				AudioHolder::GetInstance()->GetAudio(AudioIndex::Reload_SE).lock()->SoundPlayWave();
			}
		}
	}
}

void BossAction_Shot_02::Finalize() {

}

#pragma endregion

#pragma region BossAction_Jump

void BossAction_Jump::Initialize(Boss* boss) {
	boss_ = boss;

	actionTime_ = 0.0f;
	jumpingTime_ = 0.0f;
	landingTime_ = 0.0f;
	targetedCooltime_ = 0.0f;

	startPosition_ = boss_->GetTransform()->translate;

	nextPosition_.x = GameEngine::randomFloat(-(60.0f - 2.0f), (60.0f - 2.0f));
	nextPosition_.y = 2.0f;
	nextPosition_.z = GameEngine::randomFloat(-(60.0f - 2.0f), (60.0f - 2.0f));

	isEnd_ = false;
}

void BossAction_Jump::Update() {
	actionTime_ += 1.0f / 60.0f;
	if (actionTime_ > kMaxActionTime_) {
		isEnd_ = true;
	}

	if (targetedCooltime_ < kMaxTargetedCooltime_) {
		targetedCooltime_ += 1.0f / 60.0f;
		//移動方向を向かせる
		SRT* BossTransform = boss_->GetTransform();
		Vector3 diff = Normalize(Vector3(nextPosition_.x, 0.0f, nextPosition_.z) - Vector3(BossTransform->translate.x, 0.0f, BossTransform->translate.z));
		float angle = std::atan2(diff.x, diff.z);

		//最短角度補完
		float d = angle - BossTransform->rotate.y;

		if (d >= std::numbers::pi_v<float>*2) {
			d = angle - BossTransform->rotate.y;
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
		BossTransform->rotate.y = (BossTransform->rotate.y + d * 0.2f);
		BossTransform->rotate.y = std::fmodf(BossTransform->rotate.y, std::numbers::pi_v<float> *2);

		BossTransform->scale.x = 2.0f * Lerp(0.562558f, 0.562558f * 1.5f, targetedCooltime_ / kMaxTargetedCooltime_);
		BossTransform->scale.y = 2.0f * Lerp(0.562558f, 0.562558f * 0.5f, targetedCooltime_ / kMaxTargetedCooltime_);
		BossTransform->scale.z = 2.0f * Lerp(0.562558f, 0.562558f * 1.5f, targetedCooltime_ / kMaxTargetedCooltime_);

		BossTransform->translate.y = BossTransform->scale.y;
		startPosition_.y = BossTransform->translate.y;
		nextPosition_.y = BossTransform->translate.y;

	} else if (jumpingTime_ < kMaxJumpingTime_) {
		jumpingTime_ += 1.0f / 60.0f;

		SRT* BossTransform = boss_->GetTransform();

		BossTransform->translate = Lerp(startPosition_, nextPosition_, (jumpingTime_ / kMaxJumpingTime_));

		if (jumpingTime_ <= kMaxJumpingTime_ / 10) {
			BossTransform->scale.x = 2.0f * Lerp(0.562558f * 1.5f, 0.562558f * 0.5f, jumpingTime_ / (kMaxJumpingTime_ / 10));
			BossTransform->scale.y = 2.0f * Lerp(0.562558f * 0.5f, 0.562558f * 1.5f, jumpingTime_ / (kMaxJumpingTime_ / 10));
			BossTransform->scale.z = 2.0f * Lerp(0.562558f * 1.5f, 0.562558f * 0.5f, jumpingTime_ / (kMaxJumpingTime_ / 10));
		} else {
			BossTransform->scale.x = 1.0f;
			BossTransform->scale.y = 3.0f;
			BossTransform->scale.z = 1.0f;
		}

		startPosition_.y = 1;
		nextPosition_.y = 1;

		if (jumpingTime_ <= kMaxJumpingTime_ / 2) {
			BossTransform->translate.y = 2.0f * Lerp(startPosition_.y, 10.0f, 1.0f - powf(1.0f - jumpingTime_ / (kMaxJumpingTime_ / 2), 2));
		} else {
			BossTransform->translate.y = 2.0f * Lerp(nextPosition_.y, 10.0f, 1.0f - powf((jumpingTime_ - kMaxJumpingTime_ / 2) / (kMaxJumpingTime_ / 2), 2));
		}

	} else if (landingTime_ < kMaxLandingTime_) {

		if (landingTime_ <= 0.0f) {
			boss_->GetGameCamera()->SetShakeTime(0.4f);
		}

		landingTime_ += 1.0f / 60.0f;

		SRT* BossTransform = boss_->GetTransform();
		if (landingTime_ <= kMaxLandingTime_ / 2) {
			BossTransform->scale.x = 2.0f * Lerp(0.562558f * 0.5f, 0.562558f * 1.5f, (landingTime_ / (kMaxLandingTime_ / 2)));
			BossTransform->scale.y = 2.0f * Lerp(0.562558f * 1.5f, 0.562558f * 0.5f, (landingTime_ / (kMaxLandingTime_ / 2)));
			BossTransform->scale.z = 2.0f * Lerp(0.562558f * 0.5f, 0.562558f * 1.5f, (landingTime_ / (kMaxLandingTime_ / 2)));
		} else {
			BossTransform->scale.x = 2.0f * Lerp(0.562558f * 1.5f, 0.562558f, ((landingTime_ - kMaxLandingTime_ / 2) / (kMaxLandingTime_ / 2)));
			BossTransform->scale.y = 2.0f * Lerp(0.562558f * 0.5f, 0.562558f, ((landingTime_ - kMaxLandingTime_ / 2) / (kMaxLandingTime_ / 2)));
			BossTransform->scale.z = 2.0f * Lerp(0.562558f * 1.5f, 0.562558f, ((landingTime_ - kMaxLandingTime_ / 2) / (kMaxLandingTime_ / 2)));
		}

		BossTransform->translate.y = 1;
		startPosition_.y = BossTransform->translate.y;
		nextPosition_.y = BossTransform->translate.y;
	}
}

void BossAction_Jump::Finalize() {

}

#pragma endregion

#pragma region BossAction_Move

void BossAction_Move::Initialize(Boss* boss) {
	boss_ = boss;

	targetedCooltime_ = 0.0f;

	SRT* PlayerTransform = boss_->GetPlayer()->GetTransform();
	SRT* BossTransform = boss_->GetTransform();
	move_ = Normalize(Vector3(PlayerTransform->translate.x, 0.0f, PlayerTransform->translate.z) - Vector3(BossTransform->translate.x, 0.0f, BossTransform->translate.z));

	isEnd_ = false;
}

void BossAction_Move::Update() {

	if (targetedCooltime_ < kMaxTargetedCooltime_) {
		if (targetedCooltime_ <= 0) {
			AudioHolder::GetInstance()->GetAudio(AudioIndex::Reload_SE).lock()->SoundPlayWave();
		}

		targetedCooltime_ += 1.0f / 60.0f;


		float angle = std::atan2(move_.x, move_.z);

		SRT* BossTransform = boss_->GetTransform();

		//最短角度補完
		float d = angle - BossTransform->rotate.y;

		if (d >= std::numbers::pi_v<float>*2) {
			d = angle - BossTransform->rotate.y;
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
		BossTransform->rotate.y = (BossTransform->rotate.y + d * 0.2f);
		BossTransform->rotate.y = std::fmodf(BossTransform->rotate.y, std::numbers::pi_v<float> *2);
	} else {
		SRT* BossTransform = boss_->GetTransform();
		if ((BossTransform->translate.x + move_.x * speed_) > (60.0f - 2.0f) || (BossTransform->translate.x + move_.x * speed_) < -(60.0f - 2.0f) ||
			(BossTransform->translate.z + move_.z * speed_) > (60.0f - 2.0f) || (BossTransform->translate.z + move_.z * speed_) < -(60.0f - 2.0f)) {
			isEnd_ = true;
			return;
		}
		BossTransform->translate += move_ * speed_;
		boss_->GetGameCamera()->SetShakeTime(0.2f);
	}
}

void BossAction_Move::Finalize() {

}

#pragma endregion

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
	object_->SetAnimationIndex(1);
	object_->SetAnimationInterpolation(AnimationInterpolation::Cubic_Spline);
	transform_.scale = { 0.562558f * 2,0.562558f * 2,0.562558f * 2 };
	transform_.rotate = { 0.0f,0.0f,0.0f };
	transform_.translate = { 0.0f,1.0f,0.0f };
	object_->SetTransform(transform_);

	isStartAnimation_ = true;

	targetTransform_ = std::make_unique<SRT>();
	*targetTransform_ = transform_;

	action_ = std::make_unique<BossAction_Shot_01>();
	action_->Initialize(this);

	for (int& weight : weights_) {
		weight = 5;
	}

	InitializeCollider(1.0f, CollisionID_Enemy_Character);
	UpdateCollider();
}

void Boss::Update() {

	if (isStartAnimation_) {
		if (object_->IsEndAnimation()) {
			object_->SetAnimationIndex(0);
			object_->ResetAnimationTime();
			object_->SetIsLoopAnimation(true);
			isStartAnimation_ = false;
		}
	} else {

		if (HP_ > 0.0f) {

			Vector4 color;
			color = { (HP_ / maxHP_), (HP_ / maxHP_), (HP_ / maxHP_), 1.0f };
			object_->SetColor(color);

			if (HP_ / maxHP_ < 0.2f) {
				weights_[2] = 8;
				weights_[3] = 3;
			} else if (HP_ / maxHP_ < 0.5f) {
				weights_[2] = 4;
				weights_[3] = 6;
			} else {
				weights_[2] = 3;
				weights_[3] = 3;
			}

			action_->Update();
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
					action_ = std::make_unique<BossAction_Shot_02>();
				} else if (randInt <= weights_[0] + weights_[1] + weights_[2]) {
					action_ = std::make_unique<BossAction_Jump>();
				} else if (randInt <= weights_[0] + weights_[1] + weights_[2] + weights_[3]) {
					action_ = std::make_unique<BossAction_Move>();
				}
				action_->Initialize(this);
			}
			transform_ = *targetTransform_;
		}
	}
	SRT displayTransform = transform_;
	displayTransform.translate.y -= 1.0f;
	object_->SetTransform(displayTransform);
	object_->Update();
	UpdateCollider();
}

void Boss::Draw() {
	object_->Draw3D();
}

void Boss::IsCollision() {
	HP_--;
	particle_->Emit();
	gameCamera_->SetShakeTime(0.3f);
}