#include "GameEngine.h"
#include "SampleScene/SampleScene.h"
#include "GameManager.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ゲームエンジン
	GameEngine::Intialize(L"LE2A_03_オクダ_ハルト", 1280, 720);

	GameManager* gameManager = new GameManager;
	gameManager->Initialize();

	//サンプルシーン
	SampleScene* sampleScene = new SampleScene();
	sampleScene->Initialize();

	//ウィンドウの×ボタンが押されるまでループ
	while (GameEngine::WindowState()) {
		if (GameEngine::StartFlame()) {

			//
			//	更新処理
			//
			
			sampleScene->Update();
			//gameManager->Update();

			//
			//	描画処理
			//

			GameEngine::PreDraw();

			sampleScene->Draw();
			//gameManager->Draw();

			GameEngine::PostDraw();

		}
	}

	delete sampleScene;
	delete gameManager;

	GameEngine::Delete();

	return 0;
}