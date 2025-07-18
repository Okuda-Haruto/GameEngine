#include "SampleScene.h"
#include "Matrix4x4_operation.h"

SampleScene::~SampleScene() {
	//解放
	delete object_;
	delete audio_;
	delete camera_;
	delete debugCamera_;
	delete light_;
	delete axis_;

	delete grid_;
}

void SampleScene::Initialize() {

	//3Dオブジェクト
	object_ = new Object_3D;
	object_->Initialize("resources/DebugResources/multiMaterial", "multiMaterial.obj");

	audio_ = new Audio;
	audio_->Initialize("resources/DebugResources/fanfare.wav",false);

	//デバッグカメラ
	debugCamera_ = new DebugCamera();
	debugCamera_->Initialize();

	camera_ = new Camera();
	camera_->Initialize();
	object_->SetCamera(camera_);

	std::vector<ModelData> modelData = object_->GetModelData();
	for (ModelData modelDatum : modelData) {
		ObjectMaterial material;
		material.textureIndex = GameEngine::TextureLoad(modelDatum.material.textureFilePath);
		objectData_.material.push_back(material);
	}
	
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
	object_->SetLight(light_);
}

void SampleScene::Update() {
	//入力処理
	keyBord_ = GameEngine::GetKeybord();
	mouse_ = GameEngine::GetMouse();

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
	ImGui::ColorEdit4("Object Color", &objectData_.material[0].color.x);
	ImGui::DragFloat3("Object Scale", & objectData_.transform.scale.x, 0.1f);
	ImGui::SliderAngle("Object RotateX", &objectData_.transform.rotate.x);
	ImGui::SliderAngle("Object RotateY", &objectData_.transform.rotate.y);
	ImGui::SliderAngle("Object RotateZ", &objectData_.transform.rotate.z);
	ImGui::DragFloat3("Object Translate", &objectData_.transform.translate.x, 0.1f);
	for (INT i = 0; i < objectData_.material.size(); i++) {
		std::string str;
		str = "Material " + std::to_string(i) + " uvScale";
		ImGui::DragFloat3(str.c_str(), &objectData_.material[i].uvTransform.scale.x, 0.1f);
		str = "Material " + std::to_string(i) + " uvRotateX";
		ImGui::SliderAngle(str.c_str(), &objectData_.material[i].uvTransform.rotate.x);
		str = "Material " + std::to_string(i) + " uvRotateY";
		ImGui::SliderAngle(str.c_str(), &objectData_.material[i].uvTransform.rotate.y);
		str = "Material " + std::to_string(i) + " uvRotateZ";
		ImGui::SliderAngle(str.c_str(), &objectData_.material[i].uvTransform.rotate.z);
		str = "Material " + std::to_string(i) + " uvTransrate";
		ImGui::DragFloat3(str.c_str(), &objectData_.material[i].uvTransform.translate.x, 0.1f);
		str = "Material " + std::to_string(i) + " Color";
		ImGui::ColorEdit4(str.c_str(), &objectData_.material[i].color.x);
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

	grid_->IsDrawXY(isDrawXY_);
	grid_->IsDrawXZ(isDrawXZ_);
	grid_->IsDrawYZ(isDrawYZ_);

	light_->SetDirectionalLight(directionalLight);
}

void SampleScene::Draw() {
	//描画処理

	object_->Draw(GameEngine::GetCommandList(), objectData_);

	grid_->Draw(GameEngine::GetCommandList());

	axis_->Draw(GameEngine::GetCommandList());

}