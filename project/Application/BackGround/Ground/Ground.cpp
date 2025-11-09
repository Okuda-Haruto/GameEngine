#include "Ground.h"
#include "GameEngine.h"
#include "Math/Vector3_operation.h"

void Ground::Initialize() {
	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(ModelManager::GetInstance()->GetModel(1));
}

void Ground::Draw() {
	object_->Draw3D(camera_, nullptr, nullptr,nullptr);
}