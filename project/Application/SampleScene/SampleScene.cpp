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

	object_[1]->SetCubeTextureIndex(TextureManager::GetInstance()->GetSrvIndex("resources/BackGround/minedump_flats_1k.dds"));
	object_[1]->SetEnviromentCoefficient(0.5f);

	//2Dスプライト
	sprite_[0] = make_unique<Sprite>();
	sprite_[0]->Initialize("resources/DebugResources/uvChecker.png");
	sprite_[0]->SetPosition(Vector2{100.0f,100.0f});
	sprite_[1] = make_unique<Sprite>();
	sprite_[1]->Initialize("resources/DebugResources/monsterBall.png");
	sprite_[1]->SetPosition(Vector2{ 50,50 });


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

	//エフェクト
	ParticleManager::GetInstance()->CreateParticleGroup("particle", "resources/DebugResources/circle.png");
	particleEmitter_ = make_unique<ParticleEmitter>("particle");
	emitterSphere_.count = 10;
	emitterSphere_.frequency = 0.5f;
	emitterSphere_.frequencyTime = 0.0f;
	emitterSphere_.translate = Vector3(0.0f, 0.0f, 0.0f);
	emitterSphere_.radius = 1.0f;
	emitterSphere_.emit = 0;
	ParticleManager::GetInstance()->SetEmitter("particle", emitterSphere_);

	std::unique_ptr<Object> box_;
	/*box_ = make_unique<Object>();
	box_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/gltf", "LE2A_02_human.gltf"));
	box_->SetIsUseAnimation(true);
	box_->SetIsLoopAnimation(true);
	box_->SetAnimationInterpolation(AnimationInterpolation::Linear);
	gltfs_.push_back(move(box_));*/
	box_ = make_unique<Object>();
	box_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/gltf", "box.gltf"));
	box_->SetAnimationName("アーマチュアアクション");
	box_->SetIsUseAnimation(true);
	box_->SetIsLoopAnimation(true);
	box_->SetAnimationInterpolation(AnimationInterpolation::Linear);
	gltfs_.push_back(move(box_));
	box_ = make_unique<Object>();
	box_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/gltf", "box_1.gltf"));
	box_->SetAnimationName("アーマチュアアクション.001");
	box_->SetIsUseAnimation(true);
	box_->SetIsLoopAnimation(true);
	box_->SetAnimationInterpolation(AnimationInterpolation::Linear);
	gltfs_.push_back(move(box_));
	box_ = make_unique<Object>();
	box_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/gltf", "box_2.gltf"));
	box_->SetAnimationName("アーマチュアアクション.001");
	box_->SetIsUseAnimation(true);
	box_->SetIsLoopAnimation(true);
	box_->SetAnimationInterpolation(AnimationInterpolation::Linear);
	gltfs_.push_back(move(box_));
	box_ = make_unique<Object>();
	box_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/gltf", "box_0.gltf"));
	box_->SetAnimationName("アーマチュアアクション.001");
	box_->SetIsUseAnimation(true);
	box_->SetIsLoopAnimation(true);
	box_->SetAnimationInterpolation(AnimationInterpolation::Step);
	gltfs_.push_back(move(box_));
	box_ = make_unique<Object>();
	box_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/gltf", "box_0.gltf"));
	box_->SetAnimationName("アーマチュアアクション.001");
	box_->SetIsUseAnimation(true);
	box_->SetIsLoopAnimation(true);
	box_->SetAnimationInterpolation(AnimationInterpolation::Cubic_Spline);
	gltfs_.push_back(move(box_));
	box_ = make_unique<Object>();
	box_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/gltf", "box_1.gltf"));
	box_->SetAnimationName("アーマチュアアクション.001");
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

	skybox_ = std::make_unique<PrimitiveBox>();
	skybox_->Initialize(TextureManager::GetInstance()->GetSrvIndex("resources/BackGround/minedump_flats_1k.dds"),defaultCamera_,GameEngine::GetDirectXCommon());

	ring_ = std::make_unique<PrimitiveRing>();
	ring_->Initialize(TextureManager::GetInstance()->GetSrvIndex("resources/DebugResources/gradationLine.png"), defaultCamera_, GameEngine::GetDirectXCommon());
	ringTransform_ = {};
	ringTransform_.scale = { 1,1,1 };
	ringMaterial_.color = { 1.0f,1.0f,1.0f,1.0f };
	ringMaterial_.uvTransform = MakeTranslateMatrix({ 10,1.0f,1.0f });

	cylinder_ = std::make_unique<PrimitiveCylinder>();
	cylinder_->Initialize(TextureManager::GetInstance()->GetSrvIndex("resources/DebugResources/gradationLine.png"), defaultCamera_, GameEngine::GetDirectXCommon());
	cylinderTransform_ = {};
	cylinderTransform_.translate = { 20,0,0 };
	cylinderTransform_.scale = { 1,1,1 };
	cylinderMaterial_.color = { 1.0f,1.0f,1.0f,1.0f };
	cylinderMaterial_.uvTransform = MakeTranslateMatrix({ 10,1.0f,1.0f });
}

void SampleScene::Update() {
	//入力処理
	input_->Update();
	Keyboard keyboard = input_->GetKeyboard();
	Pad pad = input_->GetPad(0);

	object_[5]->Update();

	if (keyboard.keys[DIK_UP].hold || pad.Button[PAD_BUTTON_UP].hold) {
		for (INT i = 0; i < INT(objectTransform_.size()); i++) {
			objectTransform_[i].translate.y += 0.1f;
		}
	}
	if (keyboard.keys[DIK_DOWN].hold || pad.Button[PAD_BUTTON_DOWN].hold) {
		for (INT i = 0; i < INT(objectTransform_.size()); i++) {
			objectTransform_[i].translate.y -= 0.1f;
		}
	}
	if (keyboard.keys[DIK_RIGHT].hold || pad.Button[PAD_BUTTON_RIGHT].hold) {
		for (INT i = 0; i < INT(objectTransform_.size()); i++) {
			objectTransform_[i].translate.x += 0.1f;
		}
	}
	if (keyboard.keys[DIK_LEFT].hold || pad.Button[PAD_BUTTON_LEFT].hold) {
		for (INT i = 0; i < INT(objectTransform_.size()); i++) {
			objectTransform_[i].translate.x -= 0.1f;
		}
	}
	if (keyboard.keys[DIK_R].hold || pad.Button[PAD_BUTTON_BACK].trigger) {
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
	if (keyboard.keys[DIK_P].hold || pad.Button[PAD_BUTTON_RT].trigger) {
		audio_->SoundPlayWave();
	}
	if (keyboard.keys[DIK_L].hold || pad.Button[PAD_BUTTON_LT].trigger) {
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

	static SRT emitterTransform{};

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
	Primitive3DManager::GetInstance()->AddLine(line);

	static Vector3 point = { 0.0f,0.0f,0.0f };
	ImGui::DragFloat3("Point pos", &point.x, 0.01f);
	//Primitive3DManager::GetInstance()->AddPoint(point);

	static AABB aabb = { .min{-1.0f,-1.0f,-1.0f},.max{1.0f,1.0f,1.0f} };
	ImGui::DragFloat3("AABB min", &aabb.min.x, 0.01f);
	ImGui::DragFloat3("AABB max", &aabb.max.x, 0.01f);
	Primitive3DManager::GetInstance()->AddAABB(aabb);

	static Sphere sphere = { .center{0.0f,1.0f,0.0f},.radius{1.0f} };
	ImGui::DragFloat3("Sphere center", &sphere.center.x, 0.01f);
	ImGui::DragFloat("Sphere radius", &sphere.radius, 0.01f);
	Primitive3DManager::GetInstance()->AddSphere(sphere);

	if (ImGui::Button("アニメーションリセット")) {
		object_[5]->ResetTimer();
	}

	ImGui::End();

	directionalLight_->SetDirectionalLightElement(directionalLightElement_);
	pointLight_->SetPointLightElement(pointLightElement_);
	spotLight_->SetSpotLightElement(spotLightElement_);

	gltfs_[gltfIndex_]->Update();
#endif

	GameEngine::RenderPreDraw("render");

	skybox_->Draw();

	for (INT i = 0; i < object_.size(); i++) {
		if (isObjectDraw_[i]) {
			object_[i]->Draw3D();
		}
	}

	gltfs_[gltfIndex_]->Draw3D();

	particleEmitter_->Draw_AddBlend();

	if (isDisplayUI) {
		for (INT i = 0; i < sprite_.size(); i++) {
			if (isSpriteDraw_[i]) {
				sprite_[i]->Draw2D();
			}
		}
	}

	ring_->DrawBillBoard(ringTransform_, ringMaterial_);
	cylinder_->Draw(cylinderTransform_, cylinderMaterial_);

	GameEngine::RenderPostDraw();



	GameEngine::RenderPreDraw("BoxFilter");

	BoxFilterData data;
	data.scale = 5;

	GameEngine::DrawOutline("render", defaultCamera_);

	GameEngine::RenderPostDraw();
}

void SampleScene::Draw() {

	//描画処理

	//grid_->Draw(GameEngine::GetCommandList());

	//axis_->Draw(GameEngine::GetCommandList());
	
	VignetteData data;
	data.vignetteIntensity = 16.0f;
	data.vignetteCurve = 6.0f;

	GameEngine::DrawScreen("BoxFilter", data);

}