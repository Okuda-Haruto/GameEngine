#include "Ground.h"
#include "GameEngine.h"
#include "Math/Vector3_operation.h"

void Ground::Initialize(ModelHolder* modelHolder) {
	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(modelHolder->GetModel(ModelIndex::Ground));
}

void Ground::Draw() {
	object_->Draw3D();
}