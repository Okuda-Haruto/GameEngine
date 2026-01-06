#include "BackGround.h"
#include "GameEngine.h"

void BackGround::Initialize(uint32_t index) {
	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::BackGround));
	object_->SetShininess(20);
	index_ = index;
}

void BackGround::Draw() {
	object_->Draw3D(index_);
}