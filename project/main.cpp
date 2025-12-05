#include "GameEngine.h"
#include "SampleScene/SampleScene.h"
#include "GameManager.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	GameEngine::Intialize(L"LE2A_03_オクダ_ハルト", 1280, 720);
	GameManager* gameManager = new GameManager;
	gameManager->Initialize();
	//ウィンドウの×ボタンが押されるまでループ
	while (GameEngine::WindowState()) {
		if (GameEngine::StartFlame()) {

			gameManager->Update();

			GameEngine::PreDraw();

			gameManager->Draw();

			GameEngine::PostDraw();

		}
	}
	delete gameManager;
	GameEngine::Delete();
	return 0;
}