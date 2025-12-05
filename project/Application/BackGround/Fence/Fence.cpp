#include "Fence.h"
#include "GameEngine.h"

void Fence::Initialize(ModelHolder* modelHolder) {
	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(modelHolder->GetModel(ModelIndex::Fence));
}

void Fence::Draw() {
	object_->Draw3D();
}