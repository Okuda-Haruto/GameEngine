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
	delete audio_;
	delete camera_;
	delete debugCamera_;
	delete light_;
	delete axis_;

	delete grid_;
}

void SampleScene::Initialize() {

	//3Dオブジェクト
	object_[0] = new Object_3D;
	object_[0]->Initialize("resources", "Player.obj");
	object_[1] = new Object_3D;
	object_[1]->Initialize("resources/DebugResources/sphere", "sphere.obj");
	object_[2] = new Object_3D;
	object_[2]->Initialize("resources/DebugResources/multiMesh", "multiMesh.obj");
	object_[3] = new Object_3D;
	object_[3]->Initialize("resources/DebugResources/multiMaterial", "multiMaterial.obj");
	object_[4] = new Object_3D;
	object_[4]->Initialize("resources/DebugResources/teapot", "teapot.obj");
	object_[5] = new Object_3D;
	object_[5]->Initialize("resources/DebugResources/sphere", "sphere.obj");
	object_[6] = new Object_3D;
	object_[6]->Initialize("resources/DebugResources/suzanne", "suzanne.obj");

	//2Dスプライト
	sprite_ = new Sprite_2D;
	sprite_->Initialize();

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
	light_ = new Light;
	light_->Initialize();
	directionalLight = {
		{1.0f,1.0f,1.0f,1.0f},
		{0.0f,-1.0f,0.0f},
		1.0f
	};
	light_->SetDirectionalLight(directionalLight);
	isLighting_ = 2;
	for (Object_3D* object : object_) {
		object->SetLight(light_);
		object->isLighting(isLighting_);
	}
	for (INT i = 0; i < objectData_.size(); i++) {
		objectData_[i].transform.translate.x = i * 3 - 9.0f;
	}
}

void SampleScene::Update() {
	//入力処理
	keyBord_ = GameEngine::GetKeybord();
	mouse_ = GameEngine::GetMouse();
	pad_ = GameEngine::GetPad();

	if (keyBord_.hold[DIK_W] || pad_.Button[PAD_BOTTON_UP].hold) {
		for (INT i = 0; i < INT(objectData_.size());i++) {
			objectData_[i].transform.translate.y += 0.1f;
		}
	}
	if (keyBord_.hold[DIK_S] || pad_.Button[PAD_BOTTON_DOWN].hold) {
		for (INT i = 0; i < INT(objectData_.size()); i++) {
			objectData_[i].transform.translate.y -= 0.1f;
		}
	}
	if (keyBord_.hold[DIK_D] || pad_.Button[PAD_BOTTON_RIGHT].hold) {
		for (INT i = 0; i < INT(objectData_.size()); i++) {
			objectData_[i].transform.translate.x += 0.1f;
		}
	}
	if (keyBord_.hold[DIK_A] || pad_.Button[PAD_BOTTON_LEFT].hold) {
		for (INT i = 0; i < INT(objectData_.size()); i++) {
			objectData_[i].transform.translate.x -= 0.1f;
		}
	}
	if (keyBord_.hold[DIK_Q]) {
		for (INT i = 0; i < INT(objectData_.size()); i++) {
			objectData_[i].transform.rotate.y -= std::numbers::pi_v<float> / 180;
		}
	}
	if (keyBord_.hold[DIK_E]) {
		for (INT i = 0; i < INT(objectData_.size()); i++) {
			objectData_[i].transform.rotate.y += std::numbers::pi_v<float> / 180;
		}
	}
	if (keyBord_.trigger[DIK_R] || pad_.Button[PAD_BOTTON_BACK].trigger) {
		for (INT i = 0; i < INT(objectData_.size()); i++) {
			objectData_[i].transform.translate = {};
			objectData_[i].transform.rotate = {};
			objectData_[i].transform.translate.x = i * 3 - 9.0f;
		}
	}
	if (pad_.Button[PAD_BOTTON_START].trigger) {
		if (isDisplayUI) {
			isDisplayUI = false;
		} else {
			isDisplayUI = true;
		}
	}
	if (keyBord_.trigger[DIK_P] || pad_.Button[PAD_BOTTON_RT].trigger) {
		audio_->SoundPlayWave();
	}
	if (keyBord_.trigger[DIK_L] || pad_.Button[PAD_BOTTON_LT].trigger) {
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
			object->isLighting(isLighting_);
		}
	}
	if (pad_.RightStick.magnitude > 0.001) {
		Vector3 sphericalCoordinates = debugCamera_->GetSphericalCoordinates();
		sphericalCoordinates.y += pad_.RightStick.vector.x * pad_.RightStick.magnitude * 0.1f;
		if (sphericalCoordinates.y > std::numbers::pi_v<float> *2) {
			sphericalCoordinates.y -= std::numbers::pi_v<float> *2;
		} else if (sphericalCoordinates.y < -std::numbers::pi_v<float> *2) {
			sphericalCoordinates.y += std::numbers::pi_v<float> *2;
		}
		sphericalCoordinates.z += pad_.RightStick.vector.y * pad_.RightStick.magnitude * 0.1f;
		sphericalCoordinates.z = std::max(std::min(sphericalCoordinates.z, std::numbers::pi_v<float>), 0.0f);
		debugCamera_->SetSphericalCoordinates(sphericalCoordinates);
	}
	if (pad_.Button[PAD_BOTTON_LB].hold) {
		Vector3 sphericalCoordinates = debugCamera_->GetSphericalCoordinates();
		sphericalCoordinates.x -= 0.1f;
		sphericalCoordinates.x = std::min(sphericalCoordinates.x, 0.0f);
		debugCamera_->SetSphericalCoordinates(sphericalCoordinates);
	}
	if (pad_.Button[PAD_BOTTON_RB].hold) {
		Vector3 sphericalCoordinates = debugCamera_->GetSphericalCoordinates();
		sphericalCoordinates.x += 0.1f;
		debugCamera_->SetSphericalCoordinates(sphericalCoordinates);
	}


	//カメラアップデート
	if (isUseDebugCamera_) {
		camera_->Update();
	}
	if (isDisplayUI) {

		ImGui::Begin("デバッグ");
		ImGui::Checkbox("isUseDebugCamera", &isUseDebugCamera_);
		if (ImGui::Button("ResetDebugCamera")) {
			debugCamera_->Reset();
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
						object->isLighting(isLighting_);
					}
				}
			}
			ImGui::EndCombo();
		}
		ImGui::ColorEdit4("light Color", &directionalLight.color.x);
		ImGui::DragFloat3("light Direction", &directionalLight.direction.x, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("light Intensity", &directionalLight.intensity, 0.01f, 0.0f, 1.0f);
		float sqrtNumber = sqrtf(sqrtf(powf(directionalLight.direction.x, 2) + powf(directionalLight.direction.y, 2)) + powf(directionalLight.direction.z, 2));
		directionalLight.direction.x = directionalLight.direction.x / sqrtNumber;
		directionalLight.direction.y = directionalLight.direction.y / sqrtNumber;
		directionalLight.direction.z = directionalLight.direction.z / sqrtNumber;
		ImGui::Checkbox("isSpriteDraw", &isSpriteDraw_);
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
	}

	light_->SetDirectionalLight(directionalLight);
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

	if (isSpriteDraw_ && isDisplayUI) {
		sprite_->Draw(GameEngine::GetCommandList(), spriteData_);
	}

}