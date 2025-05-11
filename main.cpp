#define _USE_MATH_DEFINES
#include <cmath>

#include "GameEngine.h"
#include "GameScene.h"

//サンプル
/*
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ゲームエンジン
	GameEngine* gameEngine = new GameEngine();
	gameEngine->Intialize(L"CG2",1280,720);


	//オーディオ
	Audio* audio = new Audio();
	gameEngine->LoadAudio(audio, "resources/fanfare.wav",true);

	Audio* audio2 = new Audio();
	gameEngine->LoadAudio(audio2, "resources/Alarm01.wav",false);

	//音量
	float AudioVolume = 1.0f;


	//3Dオブジェクト
	Object_3D* object1 = new Object_3D();
	gameEngine->LoadObject(object1, "resources", "axis.obj");

	Object_3D* object2 = new Object_3D();
	gameEngine->LoadObject(object2, "resources", "plane.obj");


	//2Dオブジェクト
	Object_2D* sprite = new Object_2D();
	gameEngine->LoadObject(sprite);


	//光源
	Light* light = new Light();
	gameEngine->LoadLight(light);

	Vector4 lightColor = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 lightDirection = { 0.0f,-1.0f,0.0f };
	float lightIntensity = 1.0f;


	//デバッグカメラ
	DebugCamera* debugCamera = new DebugCamera;
	debugCamera->Initialize();

	//Transform変数を作る
	SRT transform{ {1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f} };
	//Transform変数を作る
	SRT transform2{ {1.0f,1.0f,1.0f},
		{0.0f,180.0f * float(M_PI) / 180,0.0f},
		{1.0f,0.0f,0.0f} };
	SRT transformSprite{ {1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f} };
	//カメラ変数を作る。zが-10の位置でz+の方向を向いている
	SRT cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };
	Camera camera{};


	//テクスチャ読み込み
	Texture* spriteTexture = new Texture();
	gameEngine->LoadTexture(spriteTexture, "resources/monsterBall.png");

	Texture* object3DTexture1 = new Texture();
	gameEngine->LoadTexture(object3DTexture1, (object1->ModelData()).material.textureFilePath);

	Texture* object3DTexture2 = new Texture();
	gameEngine->LoadTexture(object3DTexture2, (object2->ModelData()).material.textureFilePath);

	SRT uvTransformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	//オブジェクトの色
	Vector4 Color1 = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 Color2 = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 ColorSprite = { 1.0f,1.0f,1.0f,1.0f };



	audio2->SoundPlayWave();

	//デバッグカメラを使用するか
	bool useDebugCamera = false;

	MSG msg{};
	//ウィンドウの×ボタンが押されるまでループ
	while (gameEngine->WiodowState()) {
		if (gameEngine->StartFlame()) {

			//キーボード入力
			Keybord keybord = gameEngine->GetKeybord();
			//マウス入力
			Mouse mouse = gameEngine->GetMouse();
			//パッド入力
			Pad pad = gameEngine->GetPad(0);



			//
			//	更新処理
			//

			//ImGui
#ifdef _DEBUG
			ImGui::Begin("Debug");
			if (useDebugCamera) {
				if(ImGui::Button("debugCamera:on")) {
					useDebugCamera = false;
				}
			} else {
				if (ImGui::Button("debugCamera:off")) {
					useDebugCamera = true;
				}
				ImGui::Text("Camera");
				ImGui::DragFloat3("cameraTranslate", &(cameraTransform.translate).x, 0.01f);
				ImGui::SliderAngle("cameraRatateX", &(cameraTransform.rotate).x, -180.0f, 180.0f);
				ImGui::SliderAngle("cameraRatateY", &(cameraTransform.rotate).y, -180.0f, 180.0f);
				ImGui::SliderAngle("cameraRatateZ", &(cameraTransform.rotate).z, -180.0f, 180.0f);
			}
			ImGui::Text("material");
			ImGui::DragFloat3("materialTranslate", &(transform.translate).x, 0.01f);
			ImGui::SliderAngle("materialRatateX", &(transform.rotate).x, -180.0f, 180.0f);
			ImGui::SliderAngle("materialRatateY", &(transform.rotate).y, -180.0f, 180.0f);
			ImGui::SliderAngle("materialRatateZ", &(transform.rotate).z, -180.0f, 180.0f);
			ImGui::ColorPicker4("materialTextureColor", &Color1.x);
			ImGui::Text("material2");
			ImGui::DragFloat3("material2Translate", &(transform2.translate).x, 0.01f);
			ImGui::SliderAngle("material2RatateX", &(transform2.rotate).x, -180.0f, 180.0f);
			ImGui::SliderAngle("material2RatateY", &(transform2.rotate).y, -180.0f, 180.0f);
			ImGui::SliderAngle("material2RatateZ", &(transform2.rotate).z, -180.0f, 180.0f);
			ImGui::ColorPicker4("material2TextureColor", &Color2.x);
			ImGui::Text("sprite");
			ImGui::DragFloat3("Position", &(transformSprite.translate).x);
			ImGui::ColorPicker4("SpriteColor", &ColorSprite.x);
			ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
			ImGui::Text("directionalLiight");
			ImGui::DragFloat("intensity", &lightIntensity, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat3("direction", &lightDirection.x, 0.01f, -1.0f, 1.0f);
			ImGui::ColorPicker4("color", &lightColor.x);
			float sqrtNumber = sqrtf(sqrtf(powf(lightDirection.x, 2) + powf(lightDirection.y, 2)) + powf(lightDirection.z, 2));
			lightDirection.x = lightDirection.x / sqrtNumber;
			lightDirection.y = lightDirection.y / sqrtNumber;
			lightDirection.z = lightDirection.z / sqrtNumber;
			ImGui::Text("Audio");
			ImGui::SliderFloat("AudioVolume", &AudioVolume,0.0f,1.0f);
			ImGui::End();
#endif

			if (useDebugCamera) {
				debugCamera->Update(mouse);
				camera = debugCamera->GetTransform();
			} else {
				camera = gameEngine->UpdateCamera(cameraTransform.rotate, cameraTransform.translate);
			}

			if (keybord.hit[DIK_A]) {
				audio->SoundPlayWave();
			}
			if (keybord.hit[DIK_S]) {
				audio->SoundStopWave();
			}
			if (keybord.hit[DIK_D]) {
				audio->SoundEndWave();
			}

			object1->SetTransform(transform);
			object1->SetUVTransform(uvTransformSprite);
			object1->SetColor(Color1);
			object1->SetCamera(camera);
			object2->SetTransform(transform2);
			object2->SetUVTransform(uvTransformSprite);
			object2->SetColor(Color2);
			object2->SetCamera(camera);
			sprite->SetTransform(transformSprite);
			sprite->SetUVTransform(uvTransformSprite);
			sprite->SetColor(ColorSprite);

			light->SetColor(lightColor);
			light->SetDirection(lightDirection);
			light->SetIntensity(lightIntensity);

			audio->SetVolume(AudioVolume);
			audio2->SetVolume(AudioVolume);

			//
			//	描画処理
			//

			gameEngine->PreDraw();

			object1->Draw(gameEngine->GetCommandList(), light->directionalLightResource(), object3DTexture1->textureSrvHandleGPU());

			object2->Draw(gameEngine->GetCommandList(), light->directionalLightResource(), object3DTexture2->textureSrvHandleGPU());

			sprite->Draw(gameEngine->GetCommandList(), spriteTexture->textureSrvHandleGPU());

			gameEngine->PostDraw();
		}
	}

	delete audio;
	delete audio2;
	delete object1;
	delete object2;
	delete sprite;
	delete light;

	delete spriteTexture;
	delete object3DTexture1;
	delete object3DTexture2;

	delete gameEngine;

	return 0;
}
*/

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ゲームエンジン
	GameEngine* gameEngine = new GameEngine();
	gameEngine->Intialize(L"CG2", 1280, 720);

	GameScene* gameScene = new GameScene(gameEngine);
	gameScene->Initialize();

	MSG msg{};
	//ウィンドウの×ボタンが押されるまでループ
	while (gameEngine->WiodowState()) {
		if (gameEngine->StartFlame()) {

			//
			//	更新処理
			//

			gameScene->Update();

			//
			//	描画処理
			//

			gameScene->Draw(gameEngine);
			
		}
	}

	delete gameScene;

	delete gameEngine;

	return 0;
}