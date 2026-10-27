#include "SceneFactory.h"
#include "BaseScene/TitleScene/TitleScene.h"
#include "BaseScene/GameScene/GameScene.h"
#include "BaseScene/GameOverScene/GameOverScene.h"
#include "BaseScene/ClearScene/ClearScene.h"
#include "Editor/StageEditor/StageEditor.h"
#include "Editor/BossEditor/BossEditor.h"

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName) {
	//次のシーンを生成
	std::unique_ptr<BaseScene> newScene = nullptr;

	if (sceneName == "Title") {
		newScene = std::make_unique<TitleScene>();
	} else if (sceneName == "Game") {
		newScene = std::make_unique<GameScene>();
	} else if (sceneName == "GameOver") {
		newScene = std::make_unique<GameOverScene>();
	} else if (sceneName == "Clear") {
		newScene = std::make_unique<ClearScene>();
	} else if (sceneName == "StageEditor") {
		newScene = std::make_unique<StageEditor>();
	} else if (sceneName == "BossEditor") {
		newScene = std::make_unique<BossEditor>();
	}

	return newScene;
}