#include "Ground.h"
#include "GameEngine.h"

void Ground::Initialize() {
	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Ground));
}

void Ground::Draw() {
	object_->Draw3D();
}