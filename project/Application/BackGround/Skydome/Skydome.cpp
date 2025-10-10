#include "Skydome.h"
#include "GameEngine.h"
#include "Math/Vector3_operation.h"

void Skydome::Initialize() {
	//モデルの生成
	model_ = std::make_unique<Object_3D>();
	model_->Initialize("resources/BackGround/Skydome", "Skydome.obj");
	data_.SetMaterial(model_->GetModelData());
}

void Skydome::Draw() {
	model_->Draw(GameEngine::GetCommandList(), data_);
}