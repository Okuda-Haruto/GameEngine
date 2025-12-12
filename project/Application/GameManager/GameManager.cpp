#include "GameManager.h"
#include "GameEngine.h"

GameManager::~GameManager() {
	ModelHolder::GetInstance()->Finalize();
	SceneManager::GetInstance()->Finalize();
}

void GameManager::Initialize() {
	ModelHolder::GetInstance()->Initialize();

	sceneFactory_ = make_unique<SceneFactory>();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());
	SceneManager::GetInstance()->ChangeScene("Title");
}

void GameManager::Update() {
	SceneManager::GetInstance()->Update();
}

void GameManager::Draw() {
	SceneManager::GetInstance()->Draw();
}