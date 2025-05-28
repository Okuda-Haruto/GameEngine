#include "GameEngine.h"
#include "SampleScene.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ゲームエンジン
	GameEngine* gameEngine = new GameEngine();
	gameEngine->Intialize(L"CG2", 1280, 720);

	//サンプルシーン
	SampleScene* sampleScene = new SampleScene();
	sampleScene->Initialize(gameEngine);

	//ウィンドウの×ボタンが押されるまでループ
	while (gameEngine->WiodowState()) {
		if (gameEngine->StartFlame()) {

			//
			//	更新処理
			//
			
			sampleScene->Update();

			//
			//	描画処理
			//

			gameEngine->PreDraw();

			sampleScene->Draw();

			gameEngine->PostDraw();

		}
	}

	delete gameEngine;
	delete sampleScene;

	return 0;
}