#include "GameEngine.h"
#include "SampleScene.h"
#include "Scene/GameScene/GameScene.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ゲームエンジン
	GameEngine::Intialize(L"CG2", 1280, 720);

	//サンプルシーン
	SampleScene* sampleScene = new SampleScene();
	sampleScene->Initialize(GameEngine::GetWindowsAPI());

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