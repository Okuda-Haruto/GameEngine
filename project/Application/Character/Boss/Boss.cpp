#include "Boss.h"
#include "GameEngine.h"
#include <Vector3.h>
#include <Matrix4x4.h>
#include <numbers>
#include "Math/Lerp.h"

void Boss::Initialize(ModelHolder* modelHolder) {
	modelHolder_ = modelHolder;

	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(modelHolder_->GetModel(ModelIndex::Player));
	transform_.translate.y += 1.0f;
	object_->SetTransform(transform_);

	targetTransform_ = std::make_unique<SRT>();
	targetTransform_->scale = { 1.0f,1.0f,1.0f };
	targetTransform_->rotate = { 0.0f,0.0f,0.0f };
	targetTransform_->translate = { 0.0f,1.0f,0.0f };
}

void Boss::Update() {

}

void Boss::Draw() {
	object_->Draw3D();
}