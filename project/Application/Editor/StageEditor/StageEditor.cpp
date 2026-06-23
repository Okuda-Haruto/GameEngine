#include "StageEditor.h"
#include <ModelManager/ModelManager.h>
#include <imgui.h>
#include <fstream>
#include <cassert>
#include <GameEngine.h>
#include <Operation/Operation.h>
#include <PrimitiveManager/Primitive3DManager.h>

void StageEditor::Initialize(std::shared_ptr<Input> input) {
	input_ = input;

	commandIndex_ = 0;

	bossData_.spawnPosition = {};
	playerSpownPosition_ = { 0.0f,0.0f,-20.0f };

	debugCamera_ = std::make_shared<DebugCamera>();
	debugCamera_->Initialize(input_);
}

void StageEditor::Update() {
	if (stage_) {
		stage_->Update();
	}
	if (state_ == EditorState::None) {
		ImGui::Begin("ステージ選択");
		if (ImGui::Button("ステージ読み込み")) {
			ReadStageFile("Stage");
		}
		ImGui::End();
	} else if (state_ == EditorState::Edit) {
		ImGui::Begin("オブジェクト配置");
		//フォルダ名取得
		ImGuiFileTree("resources","resources");
		ImGui::End();

		Matrix4x4 playerMatrix = MakeAffineMatrix({1,1,1},{0,0,0},playerSpownPosition_);

		playerSpownPosition_ = { playerMatrix.m[3][0],playerMatrix.m[3][1], playerMatrix.m[3][2] };
		Primitive3DManager::GetInstance()->AddPoint(playerSpownPosition_);

		ImGui::Begin("ファイル操作");
		if (ImGui::Button("保存")) {
			WriteStageFile("Stage");
		}
		ImGui::End();
	}
}

void StageEditor::Draw() {
	if (stage_) {
		stage_->Draw();
	}
	for (auto& object : groundObjects_) {
		object.object->Draw3D();
	}
}

void StageEditor::ReadStageFile(std::string stageName) {
	stage_ = std::make_unique<Stage>();
	stage_->Initialize(stageName, input_);
	stage_->SetDebugCamera(debugCamera_);
	stageData_ = StageManager::GetInstance()->GetStageData(stageName);
	state_ = EditorState::Edit;
}

void StageEditor::WriteStageFile(std::string stageName) {
	//書き出すJsonファイル
	nlohmann::json stageJson;

	//基本ステータス
	stageJson["name"] = stageName.c_str();
	stageJson["boss"]["filePath"] = stageData_.bossData.filepath;
	stageJson["boss"]["spawnPosition"]["x"] = stageData_.bossData.spawnPosition.x;
	stageJson["boss"]["spawnPosition"]["y"] = stageData_.bossData.spawnPosition.y;
	stageJson["boss"]["spawnPosition"]["z"] = stageData_.bossData.spawnPosition.z;
	stageJson["playerSpawnPosition"]["x"] = stageData_.playerSpawnPosition.x;
	stageJson["playerSpawnPosition"]["y"] = stageData_.playerSpawnPosition.y;
	stageJson["playerSpawnPosition"]["z"] = stageData_.playerSpawnPosition.z;

	int index = 0;

	//パターン出力
	for (auto& object : groundObjects_)
	{
		if (!object.enableObject) {
			continue;
		}
		stageJson["colliderObject"][std::to_string(index)]["directoryPath"] = object.directoryPath;
		stageJson["colliderObject"][std::to_string(index)]["fileName"] = object.fileName;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["scale"]["x"] = object.transform.scale.x;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["scale"]["y"] = object.transform.scale.y;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["scale"]["z"] = object.transform.scale.z;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["rotate"]["x"] = object.transform.rotate.x;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["rotate"]["y"] = object.transform.rotate.y;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["rotate"]["z"] = object.transform.rotate.z;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["translate"]["x"] = object.transform.translate.x;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["translate"]["y"] = object.transform.translate.y;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["translate"]["z"] = object.transform.translate.z;

		index++;
	}

	//ファイル書き出し部分
	std::ofstream file(stageData_.filePath_);
	file << stageJson.dump(4);
	file.close();
}

void StageEditor::AddGroundObject(int32_t index, std::shared_ptr<Model> model, SRT transform){
	//新しく作った場合のみmodelを追加
	if (groundObjects_.size() <= index) {
		EditorObject editorObject;
		editorObject.object = std::make_shared<Object>();
		editorObject.object->Initialize(model);

		groundObjects_.push_back(std::move(editorObject));
	}

	groundObjects_[index].transform = transform;
	groundObjects_[index].enableObject = true;
}

void StageEditor::DeleteGroundObject(int32_t index) {
	assert(groundObjects_.size() <= index);
	groundObjects_[index].enableObject = false;
}

void StageEditor::SetGroundObjectPosition(int32_t index, Vector3 position) {
	assert(groundObjects_.size() <= index);
	groundObjects_[index].transform.translate = position;
}

void StageEditor::SetGroundObjectDirection(int32_t index, Vector3 direction) {
	assert(groundObjects_.size() <= index);
	groundObjects_[index].transform.rotate = direction;	//後でクォータニオンにしよう
}

void StageEditor::SetGroundObjectScale(int32_t index, Vector3 scale) {
	assert(groundObjects_.size() <= index);
	groundObjects_[index].transform.scale = scale;
}

void StageEditor::ChangeBoss(std::string name) {

}

void StageEditor::SetBossSpownPosition(Vector3 spownPosition) {
	bossData_.spawnPosition = spownPosition;
}

void StageEditor::SetPlayerSpownPosition(Vector3 spownPosition) {
	playerSpownPosition_ = spownPosition;
}

void StageEditor::ImGuiFileTree(std::string path, std::string name) {
	if (ImGui::TreeNode(name.c_str())) {
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
			{
				ImGuiFileTree(path + "/" + entry.path().filename().string(), entry.path().filename().string());
			} else if (entry.is_regular_file()) {
				//objファイル
				if (entry.path().extension() == ".obj")
				{
					if (ImGui::Button(entry.path().filename().string().c_str())) {
						AddGroundObject(currentGroundIndex_, ModelManager::GetInstance()->GetModel(path, entry.path().filename().string()) , { {1,1,1},{0,0,0},{0,0,0} });
						currentGroundIndex_++;
					}
				}
			}
		}
		ImGui::TreePop();
	}
}