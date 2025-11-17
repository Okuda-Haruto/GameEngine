#include "GameEngine.h"
#include "SampleScene/SampleScene.h"
#include "Scene/GameScene/GameScene.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ゲームエンジン
	GameEngine::Intialize(L"CG2", 1280, 720);

	//サンプルシーン
	GameScene* sampleScene = new GameScene();
	sampleScene->Initialize();

	//ウィンドウの×ボタンが押されるまでループ
	while (GameEngine::WindowState()) {
		if (GameEngine::StartFlame()) {

			//
			//	更新処理
			//
			
			sampleScene->Update();

			//
			//	描画処理
			//

			GameEngine::PreDraw();

			sampleScene->Draw();

			GameEngine::PostDraw();

		}
	}

	delete sampleScene;

	GameEngine::Delete();

	return 0;
}