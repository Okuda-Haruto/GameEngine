#include "GameEngine.h"
#include "SampleScene/SampleScene.h"
#include "GameManager.h"

using namespace std;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//ゲームエンジン
	GameEngine::Initialize(L"LE2A_03_オクダ_ハルト_GunKid", 1280, 720);

	//unique_ptr<SampleScene> gameManager = make_unique<SampleScene>();
	unique_ptr<GameManager> gameManager = make_unique<GameManager>();
	gameManager->Initialize();

	//ウィンドウの×ボタンが押されるまでループ
	while (GameEngine::WindowState()) {
		if (GameEngine::StartFlame()) {

			//
			//	更新処理
			//

			gameManager->Update();

			//
			//	描画処理
			//

			GameEngine::PreDraw();

			gameManager->Draw();

			GameEngine::PostDraw();

		}
	}

	gameManager.reset();
	GameEngine::Finalize();
	
	return 0;
}