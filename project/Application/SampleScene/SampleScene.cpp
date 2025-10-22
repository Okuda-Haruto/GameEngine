#define NOMINMAX
#include "SampleScene.h"
#include "Matrix4x4_operation.h"

#include <algorithm>
#include <numbers>
#include <cmath>

SampleScene::~SampleScene() {
	//解放
	for (Object* object : object_) {
		delete object;
	}
	delete spriteManager_;
	for (Sprite* sprite : sprite_) {
		delete sprite;
	}
	//delete effect_;
	//delete grid_;
	delete audio_;
	delete camera_;
	//delete axis_;
	delete debugCamera_;
	delete directionalLight_;
	delete pointLight_;
	delete input;
}

void SampleScene::Initialize(WindowsAPI* winApp, DirectXCommon* dxCommon) {

	winApp_ = winApp;

	//3Dオブジェクト
	object_[0] = new Object;
	object_[0]->Initialize("resources/DebugResources/plane", "plane.obj", GameEngine::GetDirectXCommon());
	object_[1] = new Object;
	object_[1]->Initialize("resources/DebugResources/sphere", "sphere.obj", GameEngine::GetDirectXCommon());
	object_[2] = new Object;
	object_[2]->Initialize("resources/DebugResources/multiMesh", "multiMesh.obj", GameEngine::GetDirectXCommon());
	object_[3] = new Object;
	object_[3]->Initialize("resources/DebugResources/terrain", "terrain.obj", GameEngine::GetDirectXCommon());
	object_[4] = new Object;
	object_[4]->Initialize("resources/DebugResources/plane", "plane.obj", GameEngine::GetDirectXCommon());
	object_[5] = new Object;
	object_[5]->Initialize("resources/DebugResources/plane", "plane.obj", GameEngine::GetDirectXCommon());
	object_[6] = new Object;
	object_[6]->Initialize("resources/DebugResources/suzanne", "suzanne.obj", GameEngine::GetDirectXCommon());

	spriteManager_ = new SpriteManager;
	spriteManager_->Initialize(dxCommon);

	//2Dスプライト
	sprite_[0] = new Sprite;
	sprite_[0]->Initialize("resources/DebugResources/uvChecker.png", spriteManager_);
	sprite_[1] = new Sprite;
	sprite_[1]->Initialize("resources/DebugResources/monsterBall.png", spriteManager_);
	sprite_[1]->SetPosition(Vector2{ 50,50 });

	//エフェクト
	//effect_ = new Effect();
	//effect_->Initialize();
	//EffectTransform.scale.x = 1.0f;

	//音源
	audio_ = new Audio;
	audio_->Initialize("resources/DebugResources/fanfare.wav",false);

	//デバッグカメラ
	debugCamera_ = new DebugCamera();
	debugCamera_->Initialize();

	//カメラ
	camera_ = new Camera();
	camera_->Initialize(GameEngine::GetDirectXCommon());
	camera_->setDebugCamera(debugCamera_);
	//for (Object* object : object_) {
	//	object->SetCamera(camera_);
	//}
	//effect_->SetCamera(camera_);

	//テクスチャ
	
	//spriteData_.material.textureIndex = GameEngine::TextureLoad("resources/Debugresources/uvChecker.png");
	
	//grid_ = new Grid;
	//grid_->Initialize(camera_);

	//axis_ = new AxisIndicator;
	//axis_->Initialize(camera_);

	//光源
	directionalLight_ = new DirectionalLight;
	directionalLight_->Initialize(GameEngine::GetDirectXCommon());
	directionalLightElement_ = {
		{1.0f,1.0f,1.0f,1.0f},
		{0.0f,-1.0f,0.0f},
		0.0f
	};
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);

	pointLight_ = new PointLight;
	pointLight_->Initialize(GameEngine::GetDirectXCommon());
	pointLightElement_ = {
		{1.0f,1.0f,1.0f,1.0f},
		{0.0f,2.0f,0.0f},
		1.0f,
		20.0f,
		1.0f
	};
	pointLight_->SetPointLightElement(pointLightElement_);
	for (INT i = 0; i < objectTransform_.size(); i++) {
		objectTransform_[i].scale = { 1.0f,1.0f,1.0f };
		objectTransform_[i].translate.x = i * 3 - 9.0f;
		object_[i]->SetTransform(objectTransform_[i]);
	}


	input = new Input;
	input->Initialize(winApp_);	//元々GameEngineでまとめて管理していたので一時的に呼び出せるようにした
}

void SampleScene::Update() {
	//入力処理
	input->Update();

	if (input->PushKey(DIK_UP) || input->PushPadButton(PAD_BUTTON_UP)) {
		for (INT i = 0; i < INT(objectTransform_.size());i++) {
			objectTransform_[i].translate.y += 0.1f;
		}
	}
	if (input->PushKey(DIK_DOWN) || input->PushPadButton(PAD_BUTTON_DOWN)) {
		for (INT i = 0; i < INT(objectTransform_.size()); i++) {
			objectTransform_[i].translate.y -= 0.1f;
		}
	}
	if (input->PushKey(DIK_RIGHT) || input->PushPadButton(PAD_BUTTON_RIGHT)) {
		for (INT i = 0; i < INT(objectTransform_.size()); i++) {
			objectTransform_[i].translate.x += 0.1f;
		}
	}
	if (input->PushKey(DIK_LEFT) || input->PushPadButton(PAD_BUTTON_LEFT)) {
		for (INT i = 0; i < INT(objectTransform_.size()); i++) {
			objectTransform_[i].translate.x -= 0.1f;
		}
	}
	if (input->PushKey(DIK_R) || input->TriggerPadButton(PAD_BUTTON_BACK)) {
		for (INT i = 0; i < INT(objectTransform_.size()); i++) {
			objectTransform_[i].translate = {};
			objectTransform_[i].rotate = {};
			objectTransform_[i].translate.x = i * 3 - 9.0f;
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

	//effect_->Update();

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
					//for (Object* object : object_) {
					//	object->SetReflection(isLighting_);
					//}
				}
			}
			ImGui::EndCombo();
		}
		ImGui::DragFloat("light Shininess", &shininess_);
		//for (Object* object : object_) {
		//	object->SetShininess(shininess);
		//}
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

		/*ImGui::Checkbox("パーティクルを発生させるか", &isSpawnEffect_);
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
		}*/
		ImGui::Checkbox("スプライト描画", &isSpriteDraw_);
		for (INT i = 0; i < sprite_.size(); i++) {
			if (isSpriteDraw_) {
				Vector2 spriteSize = sprite_[i]->GetSize();
				float spriteRotation = sprite_[i]->GetRotation();
				Vector2 spritePosition = sprite_[i]->GetPosition();
				Vector2 spriteAnchorPoint = sprite_[i]->GetAnchorPoint();
				Vector2 textureLeftTop = sprite_[i]->GetTextureLeftTop();
				Vector2 textureSize = sprite_[i]->GetTextureSize();
				Vector4 spriteColor = sprite_[i]->GetColor();
				std::string str;
				str = "Sprite[" + std::to_string(i) + "]";
				if (ImGui::CollapsingHeader(str.c_str())) {
					str = "Sprite[" + std::to_string(i) + "] Scale";
					ImGui::DragFloat2(str.c_str(), &spriteSize.x, 1.0f);
					str = "Sprite[" + std::to_string(i) + "] Rotation";
					ImGui::SliderAngle(str.c_str(), &spriteRotation);
					str = "Sprite[" + std::to_string(i) + "] Position";
					ImGui::DragFloat2(str.c_str(), &spritePosition.x, 1.0f);
					str = "Sprite[" + std::to_string(i) + "] AnchorPoint";
					ImGui::DragFloat2(str.c_str(), &spriteAnchorPoint.x, 0.1f);
					str = "Sprite[" + std::to_string(i) + "] textureLeftTop";
					ImGui::DragFloat2(str.c_str(), &textureLeftTop.x);
					str = "Sprite[" + std::to_string(i) + "] textureSize";
					ImGui::DragFloat2(str.c_str(), &textureSize.x);
					str = "Sprite[" + std::to_string(i) + "] Color";
					ImGui::ColorEdit4(str.c_str(), &spriteColor.x);
				}
				sprite_[i]->SetSize(spriteSize);
				sprite_[i]->SetRotation(spriteRotation);
				sprite_[i]->SetPosition(spritePosition);
				sprite_[i]->SetAnchorPoint(spriteAnchorPoint);
				sprite_[i]->SetTextureLeftTop(textureLeftTop);
				sprite_[i]->SetTextureSize(textureSize);
				sprite_[i]->SetColor(spriteColor);
			}
			sprite_[i]->Update();
		}
		for (INT i = 0; i < objectTransform_.size(); i++) {
			std::string str;
			str = "Object[" + std::to_string(i) + "]";
			if (ImGui::CollapsingHeader(str.c_str())) {
				str = "Object[" + std::to_string(i) + "] isDraw";
				ImGui::Checkbox(str.c_str(), &isObjectDraw_[i]);
				str = "Object[" + std::to_string(i) + "] Scale";
				ImGui::DragFloat3(str.c_str(), &objectTransform_[i].scale.x, 0.1f);
				str = "Object[" + std::to_string(i) + "] RotateX";
				ImGui::SliderAngle(str.c_str(), &objectTransform_[i].rotate.x);
				str = "Object[" + std::to_string(i) + "] RotateY";
				ImGui::SliderAngle(str.c_str(), &objectTransform_[i].rotate.y);
				str = "Object[" + std::to_string(i) + "] RotateZ";
				ImGui::SliderAngle(str.c_str(), &objectTransform_[i].rotate.z);
				str = "Object[" + std::to_string(i) + "] Translate";
				ImGui::DragFloat3(str.c_str(), &objectTransform_[i].translate.x, 0.1f);
				for (INT j = 0; j < object_[i]->GetParts().size(); j++) {
					str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " Scale";
					ImGui::DragFloat3(str.c_str(), &object_[i]->GetParts()[j].transform.scale.x, 0.1f);
					str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " RotateX";
					ImGui::SliderAngle(str.c_str(), &object_[i]->GetParts()[j].transform.rotate.x);
					str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " RotateY";
					ImGui::SliderAngle(str.c_str(), &object_[i]->GetParts()[j].transform.rotate.y);
					str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " RotateZ";
					ImGui::SliderAngle(str.c_str(), &object_[i]->GetParts()[j].transform.rotate.z);
					str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " Transrate";
					ImGui::DragFloat3(str.c_str(), &object_[i]->GetParts()[j].transform.translate.x, 0.1f);
					str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " Color";
					ImGui::ColorEdit4(str.c_str(), &object_[i]->GetParts()[j].material.color.x);
				}
			}
			object_[i]->SetTransform(objectTransform_[i]);
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

	//grid_->Draw(GameEngine::GetCommandList());

	//axis_->Draw(GameEngine::GetCommandList());

	for (INT i = 0; i < object_.size(); i++) {
		if (isObjectDraw_[i]) {
			object_[i]->Draw3D(camera_, isLighting_, shininess_,directionalLight_,pointLight_);
		}
	}

	//if (isSpawnEffect_) {
	//	effect_->Draw();
	//}

	if (isSpriteDraw_ && isDisplayUI) {
		for (INT i = 0; i < sprite_.size(); i++) {
			sprite_[i]->Draw2D();
		}
	}

}