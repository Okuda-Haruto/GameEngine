#include "GameEngine.h"
#include "SampleScene/SampleScene.h"
#include "GameManager.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace std;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//ゲームエンジン
	GameEngine::Initialize(L"LE3A_03_オクダ_ハルト_GunKid", 1280, 720);

	TextureManager::GetInstance()->MakeRenderTexture("render");
	TextureManager::GetInstance()->MakeRenderTexture("Outline");
	TextureManager::GetInstance()->MakeRenderTexture("ColorChange");
	TextureManager::GetInstance()->MakeRenderTexture("BoxFilter");
	TextureManager::GetInstance()->MakeRenderTexture("Vignette");
	TextureManager::GetInstance()->MakeRenderTexture("Dissolve");

	TextureManager::GetInstance()->LoadTexture("resources/DebugResources/noise0.png");

	//unique_ptr<SampleScene> gameManager = make_unique<SampleScene>();
	unique_ptr<GameManager> gameManager = make_unique<GameManager>();

	gameManager->Initialize();

#ifdef DEBUG
	SceneManager::GetInstance()->ChangeScene("StageEditor");
#endif // DEBUG

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