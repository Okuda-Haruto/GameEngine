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
			}
		}
	}
}

void BossAction_Shot_02::Finalize() {

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
	color_ = { 1.0f,1.0f,1.0f,1.0f };
	object_->SetColor(color_);
	transform_.scale = { 2.0f,2.0f,2.0f };
	transform_.rotate = { 0.0f,std::numbers::pi_v<float>,0.0f };
	transform_.translate = { 0.0f,2.0f,0.0f };
	object_->SetTransform(transform_);

	targetTransform_ = std::make_unique<SRT>();
	*targetTransform_ = transform_;

	action_ = std::make_unique<BossAction_Shot_01>();
	action_->Initialize(this);

	InitializeCollider(1.0f, CollisionID_Enemy_Character);
	UpdateCollider();
}

void Boss::Update() {
	if (HP_ > 0.0f) {

		color_.y = 1.0f * (HP_ / maxHP_);
		color_.z = 1.0f * (HP_ / maxHP_);

		object_->SetColor(color_);

		action_->Update();
		if (action_->IsEnd()) {
			action_->Finalize();
			action_.reset();
			action_ = std::make_unique<BossAction_Shot_02>();
			action_->Initialize(this);
		}
		transform_ = *targetTransform_;

		object_->SetTransform(transform_);
		UpdateCollider();
	}
}

void Boss::Draw() {
	if (HP_ > 0.0f) {
		object_->Draw3D();
	}
}

void Boss::IsCollision() {
	HP_--;
	particle_->Emit();
	gameCamera_->SetShakeTime(0.3f);
}