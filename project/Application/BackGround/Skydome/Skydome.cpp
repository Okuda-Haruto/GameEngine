#include "Skydome.h"
#include "GameEngine.h"
#include "Math/Vector3_operation.h"

void Skydome::Initialize(ModelHolder* modelHolder) {
	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(modelHolder->GetModel(ModelIndex::Skydome));
}

void Skydome::Draw() {
	object_->Draw3D();
}