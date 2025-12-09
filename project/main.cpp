#include "GameEngine.h"
//#include "SampleScene/SampleScene.h"
#include "GameManager.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//ゲームエンジン
	GameEngine::Intialize(L"LE2A_03_オクダ_ハルト_GunKid", 1280, 720);

	GameManager* gameManager = new GameManager;
	gameManager->Initialize();

	//SampleScene* sampleScene = new SampleScene;
	//sampleScene->Initialize();

	//ウィンドウの×ボタンが押されるまでループ
	while (GameEngine::WindowState()) {
		if (GameEngine::StartFlame()) {

			//
			//	更新処理
			//

			gameManager->Update();
			//sampleScene->Update();

			//
			//	描画処理
			//

			GameEngine::PreDraw();

			gameManager->Draw();
			//sampleScene->Draw();

			GameEngine::PostDraw();

		}
	}

	delete gameManager;
	//delete sampleScene;
	GameEngine::Delete();
	
	return 0;
}