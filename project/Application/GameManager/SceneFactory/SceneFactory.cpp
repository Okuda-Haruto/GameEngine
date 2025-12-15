#include "SceneFactory.h"
#include "Scene/TitleScene/TitleScene.h"
#include "Scene/GameScene/GameScene.h"

unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName) {
	//次のシーンを生成
	unique_ptr<BaseScene> newScene = nullptr;

	if (sceneName == "Title") {
		newScene = make_unique<TitleScene>();
	} else if (sceneName == "Game") {
		newScene = make_unique<GameScene>();
	}

	return newScene;
}