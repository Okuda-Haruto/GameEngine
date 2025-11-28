#include "Boss.h"
#include "GameEngine.h"
#include <Vector3.h>
#include <Matrix4x4.h>
#include <numbers>
#include "Math/Lerp.h"

Boss::~Boss() {

}

void Boss::Initialize(ModelHolder* modelHolder, float maxHP) {
	modelHolder_ = modelHolder;
	maxHP_ = maxHP;
	HP_ = maxHP_;

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