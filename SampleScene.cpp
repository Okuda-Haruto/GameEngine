#include "SampleScene.h"
#include "Matrix4x4_operation.h"

SampleScene::~SampleScene() {
	//解放
	delete object_;
	delete texture_;
	delete audio_;
	delete camera_;
	delete debugCamera_;
	delete light_;
	delete axis_;

	delete grid_;
}

void SampleScene::Initialize(GameEngine* gameEngine) {
	//ゲームエンジン
	gameEngine_ = gameEngine;

	//3Dオブジェクト
	object_ = new Object_3D;
	gameEngine_->LoadObject(object_,"DebugResources", "axis.obj");

	//テクスチャ
	texture_ = new Texture;
	gameEngine_->LoadTexture(texture_,object_->ModelData().material.textureFilePath);
	object_->SetTexture(texture_);

	grid_ = new Grid;
	grid_->Initialize(gameEngine_);

	audio_ = new Audio;
	gameEngine_->LoadAudio(audio_, "DebugResources/fanfare.wav",false);

	//デバッグカメラ
	debugCamera_ = new DebugCamera(gameEngine_);
	debugCamera_->Initialize();

	camera_ = new Camera(gameEngine_);
	camera_->Initialize();
	object_->SetCamera(camera_);
	grid_->SetCamera(camera_);

	axis_ = new AxisIndicator;
	axis_->Initialize(gameEngine_);
	axis_->SetCamera(camera_);

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
		debugCamera_->Update(mouse_,keyBord_);
		debugCamera_->UpdateCamera(camera_);
	}

	ImGui::Begin("Debug");
	ImGui::Checkbox("isUseDebugCamera",&isUseDebugCamera_);
	if (ImGui::Button("ResetDebugCamera")) {
		debugCamera_->Reset();
	}
	ImGui::Checkbox("isDrawXY", &isDrawXY_);
	ImGui::Checkbox("isDrawXZ", &isDrawXZ_);
	ImGui::Checkbox("isDrawYZ", &isDrawYZ_);
	
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

	if (ImGui::Button("play")) {
		audio_->SoundPlayWave();
	}
	if (ImGui::Button("stop")) {
		audio_->SoundStopWave();
	}
	if (ImGui::Button("end")) {
		audio_->SoundEndWave();
	}
	ImGui::End();

	grid_->IsDrawXY(isDrawXY_);
	grid_->IsDrawXZ(isDrawXZ_);
	grid_->IsDrawYZ(isDrawYZ_);

	light_->SetDirectionalLight(directionalLight);
}

void SampleScene::Draw() {
	//描画処理

	object_->Draw(gameEngine_->GetCommandList(), objectData_);

	grid_->Draw(gameEngine_->GetCommandList());

	axis_->Draw(gameEngine_->GetCommandList());

}