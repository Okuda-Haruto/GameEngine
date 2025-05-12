#include "GameEngine.h"
#include "GameScene.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ゲームエンジン
	GameEngine* gameEngine = new GameEngine();
	gameEngine->Intialize(L"CG2", 1280, 720);

	GameScene* gameScene = new GameScene(gameEngine);
	gameScene->Initialize();

	MSG msg{};
	//ウィンドウの×ボタンが押されるまでループ
	while (gameEngine->WiodowState()) {
		if (gameEngine->StartFlame()) {

			//
			//	更新処理
			//

			gameScene->Update();

			//
			//	描画処理
			//

			gameScene->Draw(gameEngine);
			
		}
	}

	delete gameScene;

	delete gameEngine;

	return 0;
}