#include "SceneManager.h"

SceneManager* SceneManager::instance = nullptr;

SceneManager* SceneManager::GetInstance() {
	if (instance == nullptr) {
		instance = new SceneManager;
	}
	return instance;
}

void SceneManager::Finalize() {
	//最期のシーンの終了と解放
	scene_->Finalize();
	delete scene_;
	scene_ = nullptr;

	delete instance;
	instance = nullptr;
}

void SceneManager::Update() {

	//次シーンの予約があるなら
	if (nextScene_) {
		//旧シーンの終了
		if (scene_) {
			scene_->Finalize();
			delete scene_;
			scene_ = nullptr;
		}

		//シーンの切り替え
		scene_ = nextScene_;
		nextScene_ = nullptr;

		scene_->SetSceneManager(this);

		//次シーンを初期化
		scene_->Initialize();
	}

	scene_->Update();
}

void SceneManager::Draw() {

	scene_->Draw();
}

void SceneManager::ChangeScene(string sceneName) {
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	//次のシーンを生成
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}