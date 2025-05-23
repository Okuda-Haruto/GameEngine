#include "GameEngine.h"
#include "SampleScene.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ゲームエンジン
	GameEngine* gameEngine = new GameEngine();
	gameEngine->Intialize(L"CG2", 1280, 720);

	//サンプルシーン
	SampleScene* sampleScene = new SampleScene(gameEngine);
	sampleScene->Initialize();

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

			sampleScene->Draw();

		}
	}

	delete gameEngine;
	delete sampleScene;

	return 0;
}