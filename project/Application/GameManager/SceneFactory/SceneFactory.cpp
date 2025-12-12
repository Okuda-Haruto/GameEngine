#include "SceneFactory.h"
#include "Scene/TitleScene/TitleScene.h"
#include "Scene/GameScene/GameScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName) {
	//次のシーンを生成
	BaseScene* newScene = nullptr;

	if (sceneName == "Title") {
		newScene = new TitleScene();
	} else if (sceneName == "Game") {
		newScene = new GameScene();
	}

	return newScene;
}