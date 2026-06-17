#include "GameEngine.h"
#include "SampleScene/SampleScene.h"
#include "GameManager.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace std;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//ゲームエンジン
	GameEngine::Initialize(L"LE2A_03_オクダ_ハルト_GunKid", 1280, 720);

	TextureManager::GetInstance()->MakeRenderTexture("render");
	TextureManager::GetInstance()->MakeRenderTexture("Outline");
	TextureManager::GetInstance()->MakeRenderTexture("ColorChange");
	TextureManager::GetInstance()->MakeRenderTexture("BoxFilter");
	TextureManager::GetInstance()->MakeRenderTexture("Vignette");

	//unique_ptr<SampleScene> gameManager = make_unique<SampleScene>();
	unique_ptr<GameManager> gameManager = make_unique<GameManager>();

	gameManager->Initialize();



	nlohmann::json config;

	config["WindowWidth"] = 1280;
	config["WindowHeight"] = 720;
	config["Fullscreen"] = false;

	std::cout << config.dump(4) << std::endl;

	std::ofstream file("Config.json");
	file << config.dump(4);
	file.close();

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