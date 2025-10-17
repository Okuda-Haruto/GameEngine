#include "Skydome.h"
#include "GameEngine.h"
#include "Math/Vector3_operation.h"

void Skydome::Initialize() {
	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize("resources/BackGround/Skydome", "Skydome.obj");
}

void Skydome::Draw() {
	object_->Draw3D(camera_, 0, 40,nullptr,nullptr);
}