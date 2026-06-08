#include "SceneFactory.h"
#include "BaseScene/TitleScene/TitleScene.h"
#include "BaseScene/GameScene/GameScene.h"
#include "BaseScene/GameOverScene/GameOverScene.h"
#include "BaseScene/ClearScene/ClearScene.h"

unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName) {
	//次のシーンを生成
	unique_ptr<BaseScene> newScene = nullptr;

	if (sceneName == "Title") {
		newScene = make_unique<TitleScene>();
	} else if (sceneName == "Game") {
		newScene = make_unique<GameScene>();
	} else if (sceneName == "GameOver") {
		newScene = make_unique<GameOverScene>();
	} else if (sceneName == "Clear") {
		newScene = make_unique<ClearScene>();
	}

	return newScene;
}