#include "GameManager.h"
#include "GameEngine.h"

GameManager::~GameManager() {
	delete scene_;
	scene_ = nullptr;
}

void GameManager::Initialize() {
	//モデル
	modelHolder_ = std::make_unique<ModelHolder>();
	modelHolder_->Initialize();

	//スプライト
	spriteManager_ = std::make_unique<SpriteManager>();
	spriteManager_->Initialize(GameEngine::GetDirectXCommon());

	ChangeScene(new TitleScene);
}

void GameManager::Update() {
	if (scene_->IsFinished()) {
		ChangeScene(scene_->NextScene());
	}
	scene_->Update();
}

void GameManager::Draw() {
	scene_->Draw();
}

void GameManager::ChangeScene(BaseScene* nextScene) {
	assert(nextScene != nullptr);
	delete scene_;
	scene_ = nullptr;
	scene_ = nextScene;
	scene_->Initialize(modelHolder_.get(),spriteManager_.get());
}