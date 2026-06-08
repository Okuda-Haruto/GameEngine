#include "SceneManager.h"

unique_ptr<SceneManager> SceneManager::instance;

SceneManager* SceneManager::GetInstance() {
	if (!instance) {
		instance = make_unique<SceneManager>();
	}
	return instance.get();
}

void SceneManager::Finalize() {
	//最期のシーンの終了と解放
	scene_->Finalize();
	scene_.reset();

	instance.reset();
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

void SceneManager::ChangeScene(string sceneName) {
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	//次のシーンを生成
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}