#include "Boss.h"
#include "GameEngine.h"
#include <Vector3.h>
#include <Matrix4x4.h>
#include <numbers>
#include "Math/Lerp.h"

Boss::~Boss() {

}

void Boss::Initialize(ModelHolder* modelHolder) {
	modelHolder_ = modelHolder;

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
}

void Boss::Update() {

}

void Boss::Draw() {
	object_->Draw3D();
}