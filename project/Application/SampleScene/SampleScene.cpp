#define NOMINMAX
#include "SampleScene.h"
#include "Vector3.h"
#include "Matrix4x4.h"

#include <algorithm>
#include <numbers>
#include <cmath>

using namespace std;

SampleScene::~SampleScene() {

}

void SampleScene::Initialize() {

	input_ = make_shared<Input>();
	input_->Initialize(GameEngine::GetWindowsAPI());

	ModelManager::GetInstance()->LoadModel("resources/DebugResources/plane", "plane.obj");
	ModelManager::GetInstance()->LoadModel("resources/DebugResources/sphere", "sphere.obj");
	ModelManager::GetInstance()->LoadModel("resources/DebugResources/multiMesh", "multiMesh.obj");
	ModelManager::GetInstance()->LoadModel("resources/DebugResources/multiMaterial", "multiMaterial.obj");
	ModelManager::GetInstance()->LoadModel("resources/DebugResources/terrain", "terrain.obj");
	ModelManager::GetInstance()->LoadModel("resources/DebugResources/suzanne", "suzanne.obj");
	ModelManager::GetInstance()->LoadModel("resources/DebugResources/teapot", "teapot.obj");

	//3Dオブジェクト
	object_[0] = make_unique<Object>();
	object_[0]->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/plane", "plane.obj"));
	object_[1] = make_unique<Object>();
	object_[1]->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/sphere", "sphere.obj"));
	object_[2] = make_unique<Object>();
	object_[2]->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/multiMesh", "multiMesh.obj"));
	object_[3] = make_unique<Object>();
	object_[3]->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/multiMaterial", "multiMaterial.obj"));
	object_[4] = make_unique<Object>();
	object_[4]->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/terrain", "terrain.obj"));
	object_[5] = make_unique<Object>();
	object_[5]->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/suzanne", "suzanne.obj"));
	object_[6] = make_unique<Object>();
	object_[6]->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/teapot", "teapot.obj"));

	//2Dスプライト
	sprite_[0] = make_unique<Sprite>();
	sprite_[0]->Initialize("resources/DebugResources/uvChecker.png");
	sprite_[0]->SetPosition(Vector2{100.0f,100.0f});
	sprite_[1] = make_unique<Sprite>();
	sprite_[1]->Initialize("resources/DebugResources/monsterBall.png");
	sprite_[1]->SetPosition(Vector2{ 50,50 });

	//エフェクト
	ParticleManager::GetInstance()->CreateParticleGroup("particle", "resources/DebugResources/circle.png");
	particleEmitter_ = make_unique<ParticleEmitter>("particle");
	emitter_.transform.scale = { 1.0f,1.0f,1.0f };
	emitter_.transform.translate = {0.0f,0.0f,0.0f};
	emitter_.count = 2;
	emitter_.beforeColor = { 1.0f,1.0f,1.0f,1.0f };
	emitter_.afterColor = { 1.0f,1.0f,1.0f,0.0f };
	emitter_.lifeTime = 1.0f;
	emitter_.frequency = 0.5f;
	emitter_.frequencyTime = 0.0f;
	ParticleManager::GetInstance()->SetEmitter("particle", emitter_);
	accelerationField_.area.min = { -0.5f,-0.5f,-0.5f };
	accelerationField_.area.max = { 0.5f,0.5f,0.5f };
	accelerationField_.acceleration = { 0.0f,0.0f,0.0f };
	ParticleManager::GetInstance()->SetField("particle", accelerationField_);


	//音源
	audio_ = make_unique<Audio>();
	audio_->Initialize("resources/DebugResources/TestAudio_koukaonLabo.mp3",false);

	//デバッグカメラ
	debugCamera_ = make_shared<DebugCamera>();
	debugCamera_->Initialize(input_);

	//カメラ
	defaultCamera_ = Object::GetDefaultCamera();
	defaultCamera_->SetDebugCamera(debugCamera_);

	cameraTransform_ = {
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 2.0f, 2.0f, -18.0f }
	};


	//光源
	directionalLight_ = make_shared<DirectionalLight>();
	directionalLight_->Initialize(GameEngine::GetDirectXCommon());
	directionalLightElement_ = {
		{1.0f,1.0f,1.0f,1.0f},
		{0.0f,-1.0f,0.0f},
		1.0f
	};
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);

	pointLight_ = make_shared<PointLight>();
	pointLight_->Initialize(GameEngine::GetDirectXCommon());
	pointLightElement_ = {
		{1.0f,1.0f,1.0f,1.0f},
		{0.0f,2.0f,0.0f},
		0.0f,
		20.0f,
		1.0f
	};
	pointLight_->SetPointLightElement(pointLightElement_);

	spotLight_ = make_shared<SpotLight>();
	spotLight_->Initialize(GameEngine::GetDirectXCommon());
	spotLightElement_ = {
		{ 1.0f,1.0f,1.0f,1.0f },
		{2.0f,1.25f,0.0f},
		0.0f,
		Normalize(Vector3{ -1.0f,-1.0f,0.0f }),
		4.0f,
		2.0f,
		std::numbers::pi_v<float> / 180 * 60,
		std::numbers::pi_v<float> / 180 * 30
	};
	spotLight_->SetSpotLightElement(spotLightElement_);

	for (unique_ptr<Object>& object : object_) {
		object->SetDirectionalLight(directionalLight_);
		object->SetPointLight(pointLight_);
		object->SetSpotLight(spotLight_);
	}

	for (INT i = 0; i < objectTransform_.size(); i++) {
		objectTransform_[i].scale = { 1.0f,1.0f,1.0f };
		objectTransform_[i].translate.x = i * 3 - 9.0f;
		object_[i]->SetTransform(objectTransform_[i]);
	}

	std::unique_ptr<Object> box_;
	box_ = make_unique<Object>();
	box_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/gltf", "LE2A_02_human.gltf"));
	box_->SetIsUseAnimation(true);
	box_->SetIsLoopAnimation(true);
	box_->SetAnimationInterpolation(AnimationInterpolation::Linear);
	gltfs_.push_back(move(box_));
	box_ = make_unique<Object>();
	box_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/gltf", "box.gltf"));
	box_->SetIsUseAnimation(true);
	box_->SetIsLoopAnimation(true);
	box_->SetAnimationInterpolation(AnimationInterpolation::Linear);
	gltfs_.push_back(move(box_));
	box_ = make_unique<Object>();
	box_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/gltf", "box_1.gltf"));
	box_->SetIsUseAnimation(true);
	box_->SetIsLoopAnimation(true);
	box_->SetAnimationInterpolation(AnimationInterpolation::Linear);
	gltfs_.push_back(move(box_));
	box_ = make_unique<Object>();
	box_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/gltf", "box_2.gltf"));
	box_->SetIsUseAnimation(true);
	box_->SetIsLoopAnimation(true);
	box_->SetAnimationInterpolation(AnimationInterpolation::Linear);
	gltfs_.push_back(move(box_));
	box_ = make_unique<Object>();
	box_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/gltf", "box_0.gltf"));
	box_->SetIsUseAnimation(true);
	box_->SetIsLoopAnimation(true);
	box_->SetAnimationInterpolation(AnimationInterpolation::Step);
	gltfs_.push_back(move(box_));
	box_ = make_unique<Object>();
	box_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/gltf", "box_0.gltf"));
	box_->SetIsUseAnimation(true);
	box_->SetIsLoopAnimation(true);
	box_->SetAnimationInterpolation(AnimationInterpolation::Cubic_Spline);
	gltfs_.push_back(move(box_));
	box_ = make_unique<Object>();
	box_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/gltf", "box_1.gltf"));
	box_->SetIsUseAnimation(true);
	box_->SetIsLoopAnimation(true);
	box_->SetAnimationInterpolation(AnimationInterpolation::Cubic_Spline);
	gltfs_.push_back(move(box_));

	gltfIndex_ = 0;

	for (unique_ptr<Object>& gltf : gltfs_) {
		gltf->SetDirectionalLight(directionalLight_);
		gltf->SetPointLight(pointLight_);
		gltf->SetSpotLight(spotLight_);
	}

}

void SampleScene::Update() {
	//入力処理
	input_->Update();
	Keybord keybord = input_->GetKeyBord();
	Pad pad = input_->GetPad(0);

	object_[5]->Update();

	if (keybord.hold[DIK_UP] || pad.Button[PAD_BUTTON_UP].hold) {
		for (INT i = 0; i < INT(objectTransform_.size()); i++) {
			objectTransform_[i].translate.y += 0.1f;
		}
	}
	if (keybord.hold[DIK_DOWN] || pad.Button[PAD_BUTTON_DOWN].hold) {
		for (INT i = 0; i < INT(objectTransform_.size()); i++) {
			objectTransform_[i].translate.y -= 0.1f;
		}
	}
	if (keybord.hold[DIK_RIGHT] || pad.Button[PAD_BUTTON_RIGHT].hold) {
		for (INT i = 0; i < INT(objectTransform_.size()); i++) {
			objectTransform_[i].translate.x += 0.1f;
		}
	}
	if (keybord.hold[DIK_LEFT] || pad.Button[PAD_BUTTON_LEFT].hold) {
		for (INT i = 0; i < INT(objectTransform_.size()); i++) {
			objectTransform_[i].translate.x -= 0.1f;
		}
	}
	if (keybord.hold[DIK_R] || pad.Button[PAD_BUTTON_BACK].trigger) {
		for (INT i = 0; i < INT(objectTransform_.size()); i++) {
			objectTransform_[i].translate = {};
			objectTransform_[i].rotate = {};
			objectTransform_[i].translate.x = i * 3 - 9.0f;
		}
	}
	if (pad.Button[PAD_BUTTON_START].trigger) {
		if (isDisplayUI) {
			isDisplayUI = false;
		} else {
			isDisplayUI = true;
		}
	}
	if (keybord.hold[DIK_P] || pad.Button[PAD_BUTTON_RT].trigger) {
		audio_->SoundPlayWave();
	}
	if (keybord.hold[DIK_L] || pad.Button[PAD_BUTTON_LT].trigger) {
		switch (reflection)
		{
		case 0:
			reflection = REFLECTION_None;
			break;
		case 1:
			reflection = REFLECTION_Lambert;
			break;
		case 2:
			reflection = REFLECTION_HalfLambert;
			break;
		default:
			break;
		}
	}
	if (pad.RightStick.magnitude > 0.001) {
		Vector3 sphericalCoordinates = debugCamera_->GetSphericalCoordinates();
		sphericalCoordinates.y += pad.RightStick.vector.x * pad.RightStick.magnitude * 0.1f;
		if (sphericalCoordinates.y > std::numbers::pi_v<float> *2) {
			sphericalCoordinates.y -= std::numbers::pi_v<float> *2;
		} else if (sphericalCoordinates.y < -std::numbers::pi_v<float> *2) {
			sphericalCoordinates.y += std::numbers::pi_v<float> *2;
		}
		sphericalCoordinates.z += pad.RightStick.vector.y * pad.RightStick.magnitude * 0.1f;
		sphericalCoordinates.z = std::max(std::min(sphericalCoordinates.z, std::numbers::pi_v<float>), 0.0f);
		debugCamera_->SetSphericalCoordinates(sphericalCoordinates);
	}
	if (pad.Button[PAD_BUTTON_LB].hold) {
		Vector3 sphericalCoordinates = debugCamera_->GetSphericalCoordinates();
		sphericalCoordinates.x -= 0.1f;
		sphericalCoordinates.x = std::min(sphericalCoordinates.x, 0.0f);
		debugCamera_->SetSphericalCoordinates(sphericalCoordinates);
	}
	if (pad.Button[PAD_BUTTON_RB].hold) {
		Vector3 sphericalCoordinates = debugCamera_->GetSphericalCoordinates();
		sphericalCoordinates.x += 0.1f;
		debugCamera_->SetSphericalCoordinates(sphericalCoordinates);
	}

	particleEmitter_->Update();

	//カメラアップデート
	if (isUseDebugCamera_) {
		defaultCamera_->Update();
	}
#ifdef USE_IMGUI
	if (isDisplayUI) {
		ImGui::Begin("sprite");
		ImGui::SetWindowSize("sprite", ImVec2{ 500,100 });
		Vector2 spritePosition = sprite_[0]->GetPosition();
		ImGui::DragFloat2("Position", &spritePosition.x, 1.0f, 0.0f, 0.0f, "%6.1f");	//4桁 + . + 1桁
		sprite_[0]->SetPosition(spritePosition);
		ImGui::End();
	}

	ImGui::Begin("デバッグ");
	ImGui::Text("FPS: %5.2f fps", ImGui::GetIO().Framerate);
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
	} else {
		if (ImGui::CollapsingHeader("カメラ")) {
			ImGui::DragFloat3("CameraScale", &cameraTransform_.scale.x, 0.1f);
			ImGui::SliderAngle("CameraRotateX", &cameraTransform_.rotate.x);
			ImGui::SliderAngle("CameraRotateY", &cameraTransform_.rotate.y);
			ImGui::SliderAngle("CameraRotateZ", &cameraTransform_.rotate.z);
			ImGui::DragFloat3("CameraTranslate", &cameraTransform_.translate.x, 0.1f);
		}
		defaultCamera_->Update(cameraTransform_);
	}

	const char* items[] = { "None", "Lambert", "HalfLambert" };
	static const char* current_item = "HalfLambert";

	current_item = items[reflection];

	if (ImGui::BeginCombo("Lighting", current_item))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			bool is_selected = (current_item == items[n]);
			if (ImGui::Selectable(items[n], is_selected)) {
				reflection = n;
				for (unique_ptr<Object>& object : object_) {
					object->SetReflection(reflection);
				}
			}
		}
		ImGui::EndCombo();
	}
	ImGui::DragFloat("light Shininess", &shininess_);

	const char* Shadings[] = { "Phong", "Blinn_Phong" };
	static int shadingIndex = 1;
	static const char* current_Shading = "Blinn_Phong";

	current_Shading = Shadings[shadingIndex];

	if (ImGui::BeginCombo("Shading", current_Shading))
	{
		for (int n = 0; n < IM_ARRAYSIZE(Shadings); n++)
		{
			bool is_selected = (current_Shading == Shadings[n]);
			if (ImGui::Selectable(Shadings[n], is_selected)) {
				shadingIndex = n;
				for (unique_ptr<Object>& object : object_) {
					object->SetShading(shadingIndex);
				}
			}
		}
		ImGui::EndCombo();
	}
	ImGui::DragFloat("light Shininess", &shininess_);

	for (unique_ptr<Object>& object : object_) {
		object->SetShininess(shininess_);
	}

	if (ImGui::CollapsingHeader("gltfs")) {
		ImGui::SliderInt("gltf index", &gltfIndex_, 0, int(gltfs_.size() - 1));
	}

	if (ImGui::CollapsingHeader("DirectionalLight")) {
		ImGui::ColorEdit4("directionalLight Color", &directionalLightElement_.color.x);
		ImGui::DragFloat3("directionalLight Direction", &directionalLightElement_.direction.x, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("directionalLight Intensity", &directionalLightElement_.intensity, 0.01f, 0.0f, 1.0f);
		float sqrtNumber = sqrtf(sqrtf(powf(directionalLightElement_.direction.x, 2) + powf(directionalLightElement_.direction.y, 2)) + powf(directionalLightElement_.direction.z, 2));
		directionalLightElement_.direction.x = directionalLightElement_.direction.x / sqrtNumber;
		directionalLightElement_.direction.y = directionalLightElement_.direction.y / sqrtNumber;
		directionalLightElement_.direction.z = directionalLightElement_.direction.z / sqrtNumber;
	}

	if (ImGui::CollapsingHeader("PointLight")) {
		ImGui::ColorEdit4("pointLight Color", &pointLightElement_.color.x);
		ImGui::DragFloat3("pointLight Position", &pointLightElement_.position.x, 0.1f);
		ImGui::DragFloat("pointLight Intensity", &pointLightElement_.intensity, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("pointLight Radius", &pointLightElement_.radius, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("pointLight Decay", &pointLightElement_.decay, 0.01f, 0.0f, 10.0f);
	}

	if (ImGui::CollapsingHeader("SpotLight")) {
		ImGui::ColorEdit4("spotLight Color", &spotLightElement_.color.x);
		ImGui::DragFloat3("spotLight Position", &spotLightElement_.position.x, 0.1f);
		ImGui::DragFloat("spotLight Intensity", &spotLightElement_.intensity, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat3("spotLight Direction", &spotLightElement_.direction.x, 0.01f, -1.0f, 1.0f);
		spotLightElement_.direction = Normalize(spotLightElement_.direction);
		ImGui::DragFloat("spotLight Distance", &spotLightElement_.distance, 0.1f, 0.0f, 20.0f);
		ImGui::DragFloat("spotLight Decay", &spotLightElement_.decay, 0.01f, 0.0f, 10.0f);
		ImGui::SliderAngle("spotLight CosAngle", &spotLightElement_.cosAngle);
		ImGui::SliderAngle("spotLight CosFalloutStart", &spotLightElement_.cosFalloutStart);
	}

	ImGui::DragFloat("パーティクル Scale", &emitter_.transform.scale.x, 0.1f);
	emitter_.transform.scale = { emitter_.transform.scale.x ,emitter_.transform.scale.x ,emitter_.transform.scale.x };
	ImGui::SliderAngle("パーティクル RotateX", &emitter_.transform.rotate.x);
	ImGui::SliderAngle("パーティクル RotateY", &emitter_.transform.rotate.y);
	ImGui::SliderAngle("パーティクル Rotatez", &emitter_.transform.rotate.z);
	ImGui::DragFloat3("パーティクル Translate", &emitter_.transform.translate.x, 0.1f);
	ImGui::ColorPicker4("beforecolor", &emitter_.beforeColor.x);
	ImGui::ColorPicker4("aftercolor", &emitter_.afterColor.x);
	int count = int(emitter_.count);
	ImGui::DragInt("パーティクル count", &count);
	emitter_.count = count;
	ImGui::DragFloat("パーティクル LifeTime", &emitter_.lifeTime, 0.01f);
	ImGui::DragFloat("パーティクル frequency", &emitter_.frequency, 0.01f);
	particleEmitter_->SetEmitter(emitter_);

	ImGui::DragFloat3("エリアmin", &accelerationField_.area.min.x, 0.1f);
	ImGui::DragFloat3("エリアmax", &accelerationField_.area.max.x, 0.1f);
	ImGui::DragFloat3("エリアAcceleration", &accelerationField_.acceleration.translate.x, 0.01f);
	particleEmitter_->SetField(accelerationField_);

	for (INT i = 0; i < sprite_.size(); i++) {
		std::string str;
		str = "Sprite[" + std::to_string(i) + "]";
		if (ImGui::CollapsingHeader(str.c_str())) {
			str = "描画[" + std::to_string(i) + "]";
			ImGui::Checkbox(str.c_str(), &isSpriteDraw_[i]);
			if (isSpriteDraw_[i]) {
				Vector2 spriteSize = sprite_[i]->GetSize();
				float spriteRotation = sprite_[i]->GetRotation();
				Vector2 spritePosition = sprite_[i]->GetPosition();
				Vector2 spriteAnchorPoint = sprite_[i]->GetAnchorPoint();
				Vector2 textureLeftTop = sprite_[i]->GetTextureLeftTop();
				Vector2 textureSize = sprite_[i]->GetTextureSize();
				Vector4 spriteColor = sprite_[i]->GetColor();
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
				sprite_[i]->SetSize(spriteSize);
				sprite_[i]->SetRotation(spriteRotation);
				sprite_[i]->SetPosition(spritePosition);
				sprite_[i]->SetAnchorPoint(spriteAnchorPoint);
				sprite_[i]->SetTextureLeftTop(textureLeftTop);
				sprite_[i]->SetTextureSize(textureSize);
				sprite_[i]->SetColor(spriteColor);
			}
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
				Parts parts = object_[i]->GetParts()[j];
				str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " Scale";
				ImGui::DragFloat3(str.c_str(), &parts.transform->scale.x, 0.1f);
				str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " RotateX";
				ImGui::SliderAngle(str.c_str(), &parts.transform->rotate.x);
				str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " RotateY";
				ImGui::SliderAngle(str.c_str(), &parts.transform->rotate.y);
				str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " RotateZ";
				ImGui::SliderAngle(str.c_str(), &parts.transform->rotate.z);
				str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " Transrate";
				ImGui::DragFloat3(str.c_str(), &parts.transform->translate.x, 0.1f);
				str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " UVScale";
				ImGui::DragFloat3(str.c_str(), &parts.UVtransform.scale.x, 0.1f);
				str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " UVRotateX";
				ImGui::SliderAngle(str.c_str(), &parts.UVtransform.rotate.x);
				str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " UVRotateY";
				ImGui::SliderAngle(str.c_str(), &parts.UVtransform.rotate.y);
				str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " UVRotateZ";
				ImGui::SliderAngle(str.c_str(), &parts.UVtransform.rotate.z);
				str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " UVTransrate";
				ImGui::DragFloat3(str.c_str(), &parts.UVtransform.translate.x, 0.1f);
				str = "Object[" + std::to_string(i) + "]" + "Material " + std::to_string(j) + " Color";
				ImGui::ColorEdit4(str.c_str(), &parts.material->color.x);
				object_[i]->SetParts(parts, j);
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
	static Line line = { .origin{0.0f,2.0f,0.0f},.diff{0.0f,0.0f,1.0f} };
	ImGui::DragFloat3("Line origin", &line.origin.x, 0.01f);
	ImGui::DragFloat3("Line diff", &line.diff.x, 0.01f);
	PrimitiveManager::GetInstance()->AddLine(line);

	static Vector3 point = { 0.0f,0.0f,0.0f };
	ImGui::DragFloat3("Point pos", &point.x, 0.01f);
	//PrimitiveManager::GetInstance()->AddPoint(point);

	static AABB aabb = { .min{-1.0f,-1.0f,-1.0f},.max{1.0f,1.0f,1.0f} };
	ImGui::DragFloat3("AABB min", &aabb.min.x, 0.01f);
	ImGui::DragFloat3("AABB max", &aabb.max.x, 0.01f);
	PrimitiveManager::GetInstance()->AddAABB(aabb);

	if (ImGui::Button("アニメーションリセット")) {
		object_[5]->ResetTimer();
	}

	ImGui::End();

	directionalLight_->SetDirectionalLightElement(directionalLightElement_);
	pointLight_->SetPointLightElement(pointLightElement_);
	spotLight_->SetSpotLightElement(spotLightElement_);

	gltfs_[gltfIndex_]->Update();
#endif


}

void SampleScene::Draw() {

	//描画処理

	//grid_->Draw(GameEngine::GetCommandList());

	//axis_->Draw(GameEngine::GetCommandList());

	for (INT i = 0; i < object_.size(); i++) {
		if (isObjectDraw_[i]) {
			object_[i]->Draw3D();
		}
	}

	gltfs_[gltfIndex_]->Draw3D();

	particleEmitter_->Draw();

	if (isDisplayUI) {
		for (INT i = 0; i < sprite_.size(); i++) {
			if (isSpriteDraw_[i]) {
				sprite_[i]->Draw2D();
			}
		}
	}

}