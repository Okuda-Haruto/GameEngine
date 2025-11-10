#include "Skydome.h"
#include "GameEngine.h"
#include "Math/Vector3_operation.h"

void Skydome::Initialize() {
	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(ModelManager::GetInstance()->GetModel(0));
}

void Skydome::Draw() {
	object_->Draw3D(nullptr,nullptr,nullptr);
}