#include "SceneManager.h"

void SceneManager::Finalize() {
	//最期のシーンの終了と解放
	scene_->Finalize();
	scene_.reset();
}

void SceneManager::Update() {

	//次シーンの予約があるなら
	if (nextScene_) {
		//旧シーンの終了
		if (scene_) {
			scene_->Finalize();
			scene_.reset();
		}

		//シーンの切り替え
		scene_ = move(nextScene_);
		nextScene_.reset();

		scene_->SetSceneManager(this);

		//次シーンを初期化
		scene_->Initialize(input_);
	}

	scene_->Update();
}

void SceneManager::Draw() {

	scene_->Draw();
}

void SceneManager::ChangeScene(std::string sceneName) {
	assert(sceneFactory_);

	//次のシーンを生成
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}