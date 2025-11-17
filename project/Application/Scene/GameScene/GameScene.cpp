#include "GameScene.h"
#include "GameEngine.h"
#include <numbers>

void GameScene::Initialize() {

	//モデル
	modelHolder_ = std::make_unique<ModelHolder>();
	modelHolder_->Initialize();

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(modelHolder_.get());

	//メインカメラ
	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Initialize();
	gameCamera_->SetTarget(player_->GetTransform());
	gameCamera_->SetOffset(Vector3{ 0.0f,6.0f,-60.0f });
	gameCamera_->SetRotate(Vector3{ std::numbers::pi_v<float> / 180 * 10,0.0f,0.0f });
	player_->SetCameraTransform(gameCamera_->GetTransform());
	player_->SetCamera(gameCamera_->GetCamera());

	//背景
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(modelHolder_.get());
	skydome_->SetCamera(gameCamera_->GetCamera());
	ground_ = std::make_unique<Ground>();
	ground_->Initialize(modelHolder_.get());
	ground_->SetCamera(gameCamera_->GetCamera());

	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize(GameEngine::GetDirectXCommon());
	directionalLightElement_.color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	directionalLightElement_.direction = Vector3::Normalize(Vector3{ 0.0f,-1.0f,1.0f });
	directionalLightElement_.intensity = 1.0f;
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);
	player_->SetDirectionalLight(directionalLight_.get());
	ground_->SetDirectionalLight(directionalLight_.get());
}

void GameScene::Update() {

	player_->Update();

	gameCamera_->Update();

}

void GameScene::Draw() {
	//背景
	skydome_->Draw();
	ground_->Draw();

	//プレイヤー
	player_->Draw();
}