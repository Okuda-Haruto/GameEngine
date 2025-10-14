#define NOMINMAX
#include "SampleScene.h"
#include "Matrix4x4_operation.h"

#include <algorithm>
#include <numbers>
#include <cmath>

SampleScene::~SampleScene() {
	//解放
	for (Object_3D* object : object_) {
		delete object;
	}
	delete sprite_;
	delete effect_;
	delete grid_;
	delete audio_;
	delete camera_;
	delete axis_;
	delete debugCamera_;
	delete directionalLight_;
	delete pointLight_;
	delete input;
}

void SampleScene::Initialize(WindowsAPI* winApp) {

	winApp_ = winApp;

	//3Dオブジェクト
	object_[0] = new Object_3D;
	object_[0]->Initialize("resources/DebugResources/plane", "plane.obj");
	object_[1] = new Object_3D;
	object_[1]->Initialize("resources/DebugResources/sphere", "sphere.obj");
	object_[2] = new Object_3D;
	object_[2]->Initialize("resources/DebugResources/multiMesh", "multiMesh.obj");
	object_[3] = new Object_3D;
	object_[3]->Initialize("resources/DebugResources/terrain", "terrain.obj");
	object_[4] = new Object_3D;
	object_[4]->Initialize("resources/DebugResources/teapot", "teapot.obj");
	object_[5] = new Object_3D;
	object_[5]->Initialize("resources/DebugResources/bunny", "bunny.obj");
	object_[6] = new Object_3D;
	object_[6]->Initialize("resources/DebugResources/suzanne", "suzanne.obj");

	//2Dスプライト
	sprite_ = new Sprite_2D;
	sprite_->Initialize();

	//エフェクト
	effect_ = new Effect();
	effect_->Initialize();
	EffectTransform.scale.x = 1.0f;

	//音源
	audio_ = new Audio;
	audio_->Initialize("resources/DebugResources/fanfare.wav",false);

	//デバッグカメラ
	debugCamera_ = new DebugCamera();
	debugCamera_->Initialize();

	//カメラ
	camera_ = new Camera();
	camera_->Initialize();
	camera_->setDebugCamera(debugCamera_);
	for (Object_3D* object : object_) {
		object->SetCamera(camera_);
	}
	effect_->SetCamera(camera_);

	//テクスチャ
	for (INT i = 0; i < objectData_.size(); i++) {
		objectData_[i].SetMaterial(object_[i]->GetModelData());
	}
	spriteData_.material.textureIndex = GameEngine::TextureLoad("resources/Debugresources/uvChecker.png");
	
	grid_ = new Grid;
	grid_->Initialize(camera_);

	axis_ = new AxisIndicator;
	axis_->Initialize(camera_);

	//光源
	directionalLight_ = new DirectionalLight;
	directionalLight_->Initialize();
	directionalLightElement_ = {
		{1.0f,1.0f,1.0f,1.0f},
		{0.0f,-1.0f,0.0f},
		0.0f
	};
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);

	pointLight_ = new PointLight;
	pointLight_->Initialize();
	pointLightElement_ = {
		{1.0f,1.0f,1.0f,1.0f},
		{0.0f,2.0f,0.0f},
		1.0f,
		20.0f,
		1.0f
	};
	pointLight_->SetPointLightElement(pointLightElement_);

	for (Object_3D* object : object_) {
		object->SetDirectionalLight(directionalLight_);
		object->SetPointLight(pointLight_);
		object->SetReflection(isLighting_);
	}
	for (INT i = 0; i < objectData_.size(); i++) {
		objectData_[i].transform.translate.x = i * 3 - 9.0f;
	}

	input = new Input;
	input->Initialize(winApp_);	//元々GameEngineでまとめて管理していたので一時的に呼び出せるようにした
}

void SampleScene::Update() {
	//入力処理
	input->Update();

	if (input->PushKey(DIK_UP) || input->PushPadButton(PAD_BUTTON_UP)) {
		for (INT i = 0; i < INT(objectData_.size());i++) {
			objectData_[i].transform.translate.y += 0.1f;
		}
	}
	if (input->PushKey(DIK_DOWN) || input->PushPadButton(PAD_BUTTON_DOWN)) {
		for (INT i = 0; i < INT(objectData_.size()); i++) {
			objectData_[i].transform.translate.y -= 0.1f;
		}
	}
	if (input->PushKey(DIK_RIGHT) || input->PushPadButton(PAD_BUTTON_RIGHT)) {
		for (INT i = 0; i < INT(objectData_.size()); i++) {
			objectData_[i].transform.translate.x += 0.1f;
		}
	}
	if (input->PushKey(DIK_LEFT) || input->PushPadButton(PAD_BUTTON_LEFT)) {
		for (INT i = 0; i < INT(objectData_.size()); i++) {
			objectData_[i].transform.translate.x -= 0.1f;
		}
	}
	if (input->PushKey(DIK_R) || input->TriggerPadButton(PAD_BUTTON_BACK)) {
		for (INT i = 0; i < INT(objectData_.size()); i++) {
			objectData_[i].transform.translate = {};
			objectData_[i].transform.rotate = {};
			objectData_[i].transform.translate.x = i * 3 - 9.0f;
		}
	}
	if (input->TriggerPadButton(PAD_BUTTON_START)) {
		if (isDisplayUI) {
			isDisplayUI = false;
		} else {
			isDisplayUI = true;
		}
	}
	if (input->PushKey(DIK_P) || input->TriggerPadButton(PAD_BUTTON_RT)) {
		audio_->SoundPlayWave();
	}
	if (input->PushKey(DIK_L) || input->TriggerPadButton(PAD_BUTTON_LT)) {
		switch (isLighting_)
		{
		case 0:
			isLighting_ = 1;
			break;
		case 1:
			isLighting_ = 2;
			break;
		case 2:
			isLighting_ = 0;
			break;
		default:
			break;
		}
		for (Object_3D* object : object_) {
			object->SetReflection(isLighting_);
		}
	}
	if (input->PadRightStick().magnitude > 0.001) {
		Vector3 sphericalCoordinates = debugCamera_->GetSphericalCoordinates();
		sphericalCoordinates.y += input->PadRightStick().vector.x * input->PadRightStick().magnitude * 0.1f;
		if (sphericalCoordinates.y > std::numbers::pi_v<float> *2) {
			sphericalCoordinates.y -= std::numbers::pi_v<float> *2;
		} else if (sphericalCoordinates.y < -std::numbers::pi_v<float> *2) {
			sphericalCoordinates.y += std::numbers::pi_v<float> *2;
		}
		sphericalCoordinates.z += input->PadRightStick().vector.y * input->PadRightStick().magnitude * 0.1f;
		sphericalCoordinates.z = std::max(std::min(sphericalCoordinates.z, std::numbers::pi_v<float>), 0.0f);
		debugCamera_->SetSphericalCoordinates(sphericalCoordinates);
	}
	if (input->PushPadButton(PAD_BUTTON_LB)) {
		Vector3 sphericalCoordinates = debugCamera_->GetSphericalCoordinates();
		sphericalCoordinates.x -= 0.1f;
		sphericalCoordinates.x = std::min(sphericalCoordinates.x, 0.0f);
		debugCamera_->SetSphericalCoordinates(sphericalCoordinates);
	}
	if (input->PushPadButton(PAD_BUTTON_RB)) {
		Vector3 sphericalCoordinates = debugCamera_->GetSphericalCoordinates();
		sphericalCoordinates.x += 0.1f;
		debugCamera_->SetSphericalCoordinates(sphericalCoordinates);
	}

	effect_->Update();

	//カメラアップデート
	if (isUseDebugCamera_) {
		camera_->Update();
	}
	if (isDisplayUI) {

		ImGui::Begin("デバッグ");
		ImGui::Checkbox("デバッグカメラ", &isUseDebugCamera_);
		if (isUseDebugCamera_) {
			if (ImGui::Button("カメラリセット")) {
				debugCamera_->Reset();
			}

			const char* cameraMode[] = { "PlayerCamera", "SphericalCoordinates" };
			static int modeNum = 1;
			static const char* current_Mode = "SphericalCoordinates";

			current_Mode = cameraMode[modeNum];

			if (ImGui::BeginCombo("DebugCameraMode", current_Mode))
			{
				for (int n = 0; n < IM_ARRAYSIZE(cameraMode); n++)
				{
					bool is_selected = (current_Mode == cameraMode[n]);
					if (ImGui::Selectable(cameraMode[n], is_selected)) {
						modeNum = n;
						switch (n)
						{
						case 0:
							debugCamera_->ChangeCameraMode(DebugCameraMode::PlayerCamera);
							break;
						case 1:
							debugCamera_->ChangeCameraMode(DebugCameraMode::SphericalCoordinates);
							break;
						default:
							break;
						}
					}
				}
				ImGui::EndCombo();
			}
		}

		const char* items[] = { "None", "Lambert", "HalfLambert" };
		static const char* current_item = "HalfLambert";

		current_item = items[isLighting_];

		if (ImGui::BeginCombo("Lighting", current_item))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				bool is_selected = (current_item == items[n]);
				if (ImGui::Selectable(items[n], is_selected)) {
					isLighting_ = n;
					for (Object_3D* object : object_) {
						object->SetReflection(isLighting_);
					}
				}
			}
			ImGui::EndCombo();
		}
		static float shininess = 40.0f;
		ImGui::DragFloat("light Shininess", &shininess);
		for (Object_3D* object : object_) {
			object->SetShininess(shininess);
		}
		ImGui::ColorEdit4("directionalLight Color", &directionalLightElement_.color.x);
		ImGui::DragFloat3("directionalLight Direction", &directionalLightElement_.direction.x, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("directionalLight Intensity", &directionalLightElement_.intensity, 0.01f, 0.0f, 1.0f);
		float sqrtNumber = sqrtf(sqrtf(powf(directionalLightElement_.direction.x, 2) + powf(directionalLightElement_.direction.y, 2)) + powf(directionalLightElement_.direction.z, 2));
		directionalLightElement_.direction.x = directionalLightElement_.direction.x / sqrtNumber;
		directionalLightElement_.direction.y = directionalLightElement_.direction.y / sqrtNumber;
		directionalLightElement_.direction.z = directionalLightElement_.direction.z / sqrtNumber;

		ImGui::ColorEdit4("pointLight Color", &pointLightElement_.color.x);
		ImGui::DragFloat3("pointLight Position", &pointLightElement_.position.x, 0.1f);
		ImGui::DragFloat("pointLight Intensity", &pointLightElement_.intensity, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("pointLight Radius", &pointLightElement_.radius, 0.01f, 0.0f,100.0f);
		ImGui::DragFloat("pointLight Decay", &pointLightElement_.decay, 0.01f, 0.0f,10.0f);

		ImGui::Checkbox("パーティクルを発生させるか", &isSpawnEffect_);
		if (isSpawnEffect_) {
			ImGui::Checkbox("fieldを使用するか", &isUseField);
			effect_->IsUseField(isUseField);
			ImGui::DragFloat("パーティクル Scale", &EffectTransform.scale.x, 0.1f);
			EffectTransform.scale = { EffectTransform.scale.x ,EffectTransform.scale.x ,EffectTransform.scale.x };
			ImGui::SliderAngle("パーティクル RotateX", &EffectTransform.rotate.x);
			ImGui::SliderAngle("パーティクル RotateY", &EffectTransform.rotate.y);
			ImGui::SliderAngle("パーティクル Rotatez", &EffectTransform.rotate.z);
			ImGui::DragFloat3("パーティクル Translate", &EffectTransform.translate.x, 0.1f);
			ImGui::ColorPicker4("color", &EffectColor.x);
			effect_->SetColor(EffectColor);
			effect_->SetTransform(EffectTransform);
		}
		ImGui::Checkbox("スプライト描画", &isSpriteDraw_);
		if (isSpriteDraw_) {
			ImGui::DragFloat3("Sprite Scale", &spriteData_.transform.scale.x, 0.1f);
			ImGui::SliderAngle("Sprite RotateX", &spriteData_.transform.rotate.x);
			ImGui::SliderAngle("Sprite RotateY", &spriteData_.transform.rotate.y);
			ImGui::SliderAngle("Sprite RotateZ", &spriteData_.transform.rotate.z);
			ImGui::DragFloat3("Sprite Transrate", &spriteData_.transform.translate.x, 0.1f);
			ImGui::DragFloat3("Sprite uvScale", &spriteData_.material.uvTransform.scale.x, 0.1f);
			ImGui::SliderAngle("Sprite uvRotateX", &spriteData_.material.uvTransform.rotate.x);
			ImGui::SliderAngle("Sprite uvRotateY", &spriteData_.material.uvTransform.rotate.y);
			ImGui::SliderAngle("Sprite uvRotateZ", &spriteData_.material.uvTransform.rotate.z);
			ImGui::DragFloat3("Sprite uvTransrate", &spriteData_.material.uvTransform.translate.x, 0.1f);
			ImGui::ColorEdit4("Sprite Color", &spriteData_.material.color.x);
		}
		for (INT i = 0; i < objectData_.size(); i++) {
			std::string str;
			str = "Object[" + std::to_string(i) + "]";
			if (ImGui::CollapsingHeader(str.c_str())) {
				str = "Object[" + std::to_string(i) + "] isDraw";
				ImGui::Checkbox(str.c_str(), &isObjectDraw_[i]);
				str = "Object[" + std::to_string(i) + "] Scale";
				ImGui::DragFloat3(str.c_str(), &objectData_[i].transform.scale.x, 0.1f);
				str = "Object[" + std::to_string(i) + "] RotateX";
				ImGui::SliderAngle(str.c_str(), &objectData_[i].transform.rotate.x);
				str = "Object[" + std::to_string(i) + "] RotateY";
				ImGui::SliderAngle(str.c_str(), &objectData_[i].transform.rotate.y);
				str = "Object[" + std::to_string(i) + "] RotateZ";
				ImGui::SliderAngle(str.c_str(), &objectData_[i].transform.rotate.z);
				str = "Object[" + std::to_string(i) + "] Translate";
				ImGui::DragFloat3(str.c_str(), &objectData_[i].transform.translate.x, 0.1f);
				for (INT j = 0; j < objectData_[i].material.size(); j++) {
					str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " uvScale";
					ImGui::DragFloat3(str.c_str(), &objectData_[i].material[j].uvTransform.scale.x, 0.1f);
					str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " uvRotateX";
					ImGui::SliderAngle(str.c_str(), &objectData_[i].material[j].uvTransform.rotate.x);
					str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " uvRotateY";
					ImGui::SliderAngle(str.c_str(), &objectData_[i].material[j].uvTransform.rotate.y);
					str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " uvRotateZ";
					ImGui::SliderAngle(str.c_str(), &objectData_[i].material[j].uvTransform.rotate.z);
					str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " uvTransrate";
					ImGui::DragFloat3(str.c_str(), &objectData_[i].material[j].uvTransform.translate.x, 0.1f);
					str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " Color";
					ImGui::ColorEdit4(str.c_str(), &objectData_[i].material[j].color.x);
				}
			}
		}

		if (ImGui::Button("オーディオ再生")) {
			audio_->SoundPlayWave();
		}
		if (ImGui::Button("オーディオ停止")) {
			audio_->SoundStopWave();
		}
		if (ImGui::Button("オーディオ終了")) {
			audio_->SoundEndWave();
		}
		ImGui::End();
	}

	directionalLight_->SetDirectionalLightElement(directionalLightElement_);
	pointLight_->SetPointLightElement(pointLightElement_);
}

void SampleScene::Draw() {

	//描画処理

	grid_->Draw(GameEngine::GetCommandList());

	axis_->Draw(GameEngine::GetCommandList());

	for (INT i = 0; i < object_.size(); i++) {
		if (isObjectDraw_[i]) {
			object_[i]->Draw(GameEngine::GetCommandList(), objectData_[i]);
		}
	}

	if (isSpawnEffect_) {
		effect_->Draw();
	}

	if (isSpriteDraw_ && isDisplayUI) {
		sprite_->Draw(GameEngine::GetCommandList(), spriteData_);
	}

}