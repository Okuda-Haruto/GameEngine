#include "Skydome.h"
#include "GameEngine.h"

void Skydome::Initialize(std::shared_ptr<Camera> camera) {
	//モデルの生成
	skybox_ = std::make_unique<PrimitiveBox>();
	skybox_->Initialize(TextureManager::GetInstance()->GetSrvIndex("resources/BackGround/minedump_flats_1k.dds"), camera, GameEngine::GetDirectXCommon());
}

void Skydome::Draw() {
	skybox_->Draw();
}