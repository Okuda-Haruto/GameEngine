#include "GameEngine.h"
//#include "SampleScene/SampleScene.h"
#include "GameManager.h"

using namespace std;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//ゲームエンジン
	GameEngine::Intialize(L"LE2A_03_オクダ_ハルト_GunKid", 1280, 720);

	unique_ptr<GameManager> gameManager = make_unique<GameManager>();
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

	gameManager.reset();
	//delete sampleScene;
	GameEngine::Finalize();
	
	return 0;
}