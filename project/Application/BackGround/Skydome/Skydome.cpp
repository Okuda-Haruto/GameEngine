#include "Skydome.h"
#include "GameEngine.h"

void Skydome::Initialize(ModelHolder* modelHolder) {
	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(modelHolder->GetModel(ModelIndex::Skydome));
	object_->SetReflection(REFLECTION_None);
}

void Skydome::Draw() {
	object_->Draw3D();
}