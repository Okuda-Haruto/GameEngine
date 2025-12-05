#include "Boss.h"
#include "GameEngine.h"
#include <Vector3.h>
#include <Matrix4x4.h>
#include <numbers>
#include "Math/Lerp.h"
#include "Scene/GameScene/GameScene.h"
#include "Operation/Operation.h"

Boss::~Boss() {

}

void Boss::Initialize(ModelHolder* modelHolder, GameScene* gameScene, Player* player, float maxHP) {
	modelHolder_ = modelHolder;
	maxHP_ = maxHP;
	HP_ = maxHP_;

	gameScene_ = gameScene;
	player_ = player;

	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(modelHolder_->GetModel(ModelIndex::Player));
	object_->SetColor(Vector4{ 1.0f,0.0f,0.0f,1.0f });
	transform_.scale = { 2.0f,2.0f,2.0f };
	transform_.rotate = { 0.0f,std::numbers::pi_v<float>,0.0f };
	transform_.translate = { 0.0f,2.0f,0.0f };
	object_->SetTransform(transform_);

	targetTransform_ = std::make_unique<SRT>();
	*targetTransform_ = transform_;

	InitializeCollider(1.0f, CollisionID_Enemy_Character);
	UpdateCollider();
}

void Boss::Update() {

	if (shotCooltime_ < kMaxShotCooltime) {
		shotCooltime_ += 1.0f / 60.0f;
	}
	if (shotCooltime_ >= kMaxShotCooltime) {
		Vector3 translate = player_->GetTransform()->translate;
		Vector3 diff = Normalize(Vector3(translate.x,0.0f,translate.z) - Vector3(transform_.translate.x,0.0f,transform_.translate.z));
		Vector3 rotate;
		//  Y軸回り回転(θy)
		rotate.y = std::atan2(diff.x, diff.z);
		float length = Length(Vector3{ diff.x, 0.0f, diff.z });
		// X軸回り回転(θx)
		rotate.x = std::atan2(-diff.y, length);
		rotate.z = 0.0f;

		gameScene_->AddBossBullet(transform_.translate, rotate);
		shotCooltime_ = 0.0f;
	}

	object_->SetTransform(transform_);
	UpdateCollider();
}

void Boss::Draw() {
	if (HP_ > 0.0f) {
		object_->Draw3D();
	}
}

void Boss::IsCollision() {
	HP_--;
}