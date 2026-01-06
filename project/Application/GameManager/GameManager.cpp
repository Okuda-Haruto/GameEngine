#include "GameManager.h"
#include "GameEngine.h"

GameManager::~GameManager() {
	ModelHolder::GetInstance()->Finalize();
	AudioHolder::GetInstance()->Finalize();
	SceneManager::GetInstance()->Finalize();
}

void GameManager::Initialize() {
	ModelHolder::GetInstance()->Initialize();
	AudioHolder::GetInstance()->Initialize();

	input_ = make_shared<Input>();
	input_.get()->Initialize(GameEngine::GetWindowsAPI());

	sceneFactory_ = make_unique<SceneFactory>();
	SceneManager::GetInstance()->SetInput(input_);
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());
	SceneManager::GetInstance()->ChangeScene("Title");
}

void GameManager::Update() {
	input_->Update();
	SceneManager::GetInstance()->Update();
}

void GameManager::Draw() {
	SceneManager::GetInstance()->Draw();
}