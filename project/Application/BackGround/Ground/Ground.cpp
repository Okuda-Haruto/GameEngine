#include "Ground.h"
#include "GameEngine.h"
#include "Math/Vector3_operation.h"

void Ground::Initialize() {
	//モデルの生成
	model_ = std::make_unique<Object_3D>();
	model_->Initialize("resources/BackGround/Ground", "Ground.obj");
	data_.SetMaterial(model_->GetModelData());
}

void Ground::Draw() {
	model_->Draw(GameEngine::GetCommandList(), data_);
}