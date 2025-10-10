#include "GameScene.h"
#include "GameEngine.h"

void GameScene::Initialize() {

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize();

	//メインカメラ
	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Initialize();
	gameCamera_->SetTarget(player_->GetTransform());
	player_->SetCameraTransform(gameCamera_->GetTransform());
	player_->SetCamera(gameCamera_->GetCamera());

	//背景
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();
	skydome_->SetCamera(gameCamera_->GetCamera());
	ground_ = std::make_unique<Ground>();
	ground_->Initialize();
	ground_->SetCamera(gameCamera_->GetCamera());
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