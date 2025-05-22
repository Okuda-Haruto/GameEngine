#include "GameEngine.h"
#include "GameScene.h"
#include "Vector3_operation.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ゲームエンジン
	GameEngine* gameEngine = new GameEngine();
	gameEngine->Intialize(L"CG2", 1280, 720);

	//GameScene* gameScene = new GameScene(gameEngine);
	//gameScene->Initialize();

	Object_3D* object = new Object_3D();
	gameEngine->LoadObject(object,"resources","axis.obj");

	Texture* texture = new Texture();
	gameEngine->LoadTexture(texture, object->ModelData().material.textureFilePath);
	object->SetTexture(texture);

	//Text_2D* sprite = new Text_2D();
	//gameEngine->LoadObject(sprite);

	//Text* text = new Text();
	//gameEngine->LoadText(text, 80, FW_NORMAL, L"#これはテストメッセージです。\n #C[0xAA0000]Color#C[0xFFFFFF]と #S[160]Size#S[80]と #W[700]Weight#W[400]を変えられます。", "resources/Ronde-B_square.otf", "ロンド B スクエア");

	//sprite->SetText(text->textData());

	//Object_2D_Data data{};
	//data.transform = {
	//	{1.0f,1.0f,1.0f},
	//	{0.0f,0.0f,0.0f},
	//	{0.0f,0.0f,0.0f}
	//};
	//data.uvTransform = {
	//{1.0f,1.0f,1.0f},
	//{0.0f,0.0f,0.0f},
	//{0.0f,0.0f,0.0f}
	//};
	//data.color = { 1.0f,1.0f,1.0f,1.0f };

	Object_3D_Data data3D{};
	data3D.transform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	data3D.uvTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	data3D.color = { 1.0f,1.0f,1.0f,1.0f };

	data3D.camera = gameEngine->UpdateCamera({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-10.0f });
	Light* light = new Light;
	gameEngine->LoadLight(light);
	Vector4 lightColor = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 lightDirection = { 0.0f,-1.0f,0.0f };
	float lightIntensity = 1.0f;

	//Vector2 SpriteSize = { 100,220 };

	//int index = 0;

	//int flame = 0;
	//int indexFlame = 15;
	//int maxflame = (text->GetTextlength() - 1) * indexFlame;

	//ウィンドウの×ボタンが押されるまでループ
	while (gameEngine->WiodowState()) {
		if (gameEngine->StartFlame()) {

			//
			//	更新処理
			//

			ImGui::ColorEdit4("lightColor", &lightColor.x);
			ImGui::DragFloat3("lightDirection", &lightDirection.x, 0.01f, -1.0f, 1.0f);
			ImGui::DragFloat("lightIntensity", &lightIntensity,0.01f,0.0f,1.0f);
			float sqrtNumber = sqrtf(sqrtf(powf(lightDirection.x, 2) + powf(lightDirection.y, 2)) + powf(lightDirection.z, 2));
			lightDirection.x = lightDirection.x / sqrtNumber;
			lightDirection.y = lightDirection.y / sqrtNumber;
			lightDirection.z = lightDirection.z / sqrtNumber;
			ImGui::ColorEdit4("Object Color", &data3D.color.x);
			ImGui::SliderAngle("Object RotateX", &data3D.transform.rotate.x);
			ImGui::SliderAngle("Object RotateY", &data3D.transform.rotate.y);
			ImGui::SliderAngle("Object RotateZ", &data3D.transform.rotate.z);

			light->SetColor(lightColor);
			light->SetDirection(lightDirection);
			light->SetIntensity(lightIntensity);
			object->SetLight(light);
			

			//maxflame = (text->GetTextlength() - 1) * indexFlame;

			//if (flame < maxflame) {
			//	flame++;
			//}
			//index = (flame / indexFlame) + 1;

			//gameScene->Update();

			//
			//	描画処理
			//

			//sprite->SetSpriteSize(SpriteSize.x, SpriteSize.y);

			gameEngine->PreDraw();


			object->Draw(gameEngine->GetCommandList(), data3D);

			//sprite->Draw(gameEngine->GetCommandList(), data, index);

			//gameScene->Draw(gameEngine);
			
			gameEngine->PostDraw();

		}
	}

	//delete gameScene;

	//delete text;

	//delete sprite;

	delete object;
	delete texture;
	delete light;

	delete gameEngine;

	return 0;
}