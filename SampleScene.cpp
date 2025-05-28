#include "SampleScene.h"

SampleScene::~SampleScene() {
	//解放
	delete object_;
	delete texture_;
	delete debugCamera_;
	delete light_;
}

void SampleScene::Initialize(GameEngine* gameEngine) {
	//ゲームエンジン
	gameEngine_ = gameEngine;

	//3Dオブジェクト
	object_ = new Sprite_3D;
	gameEngine_->LoadObject(object_);

	//テクスチャ
	texture_ = new Texture;
	gameEngine_->LoadTexture(texture_, "DebugResources/UVChecker.png");
	object_->SetTexture(texture_);

	//デバッグカメラ
	debugCamera_ = new DebugCamera;
	gameEngine_->InitializeDebugCamera(debugCamera_);

	camera_ = gameEngine_->UpdateCamera(
		{
			{1.0f,1.0f,1.0f},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,-10.0f}
		});

	//光源
	light_ = new Light;
	gameEngine_->LoadLight(light_);
	directionalLight = {
		{1.0f,1.0f,1.0f,1.0f},
		{0.0f,-1.0f,0.0f},
		1.0f
	};
	light_->SetDirectionalLight(directionalLight);
	object_->SetLight(light_);
}

void SampleScene::Update() {
	//入力処理
	keyBord_ = gameEngine_->GetKeybord();
	mouse_ = gameEngine_->GetMouse();

	//カメラアップデート
	if (isUseDebugCamera_) {
		debugCamera_->Update(mouse_);
		camera_ = debugCamera_->GetCamera();
	}

	ImGui::Begin("Debug");
	ImGui::Checkbox("isUseDebugCamera",&isUseDebugCamera_);
	if (ImGui::Button("ResetDebugCamera")) {
		debugCamera_->Reset();
	}
	ImGui::ColorEdit4("light Color", &directionalLight.color.x);
	ImGui::DragFloat3("light Direction", &directionalLight.direction.x, 0.01f, -1.0f, 1.0f);
	ImGui::DragFloat("light Intensity", &directionalLight.intensity, 0.01f, 0.0f, 1.0f);
	float sqrtNumber = sqrtf(sqrtf(powf(directionalLight.direction.x, 2) + powf(directionalLight.direction.y, 2)) + powf(directionalLight.direction.z, 2));
	directionalLight.direction.x = directionalLight.direction.x / sqrtNumber;
	directionalLight.direction.y = directionalLight.direction.y / sqrtNumber;
	directionalLight.direction.z = directionalLight.direction.z / sqrtNumber;
	ImGui::ColorEdit4("Object Color", &objectData_.color.x);
	ImGui::DragFloat3("Object Scale", & objectData_.transform.scale.x, 0.1f);
	ImGui::SliderAngle("Object RotateX", &objectData_.transform.rotate.x);
	ImGui::SliderAngle("Object RotateY", &objectData_.transform.rotate.y);
	ImGui::SliderAngle("Object RotateZ", &objectData_.transform.rotate.z);
	ImGui::DragFloat3("Object Translate", &objectData_.transform.translate.x, 0.1f);
	ImGui::End();

	light_->SetDirectionalLight(directionalLight);
	objectData_.camera = camera_;
}

void SampleScene::Draw() {
	//描画処理

	object_->Draw(gameEngine_->GetCommandList(), objectData_);

}