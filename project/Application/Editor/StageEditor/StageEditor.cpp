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
			WriteStageFile();
		}
		ImGui::End();
	}
}

void StageEditor::Draw() {
	if (stage_) {
		stage_->Draw();
	}
	for (auto& object : colliderObjects_) {
		object.object->Draw3D();
	}
}

void StageEditor::ReadStageFile(std::string stageName) {
	stageName_ = stageName;
	stage_ = std::make_unique<Stage>();
	stage_->Initialize(stageName_, input_);
	stage_->SetDebugCamera(debugCamera_);
	stageData_ = StageManager::GetInstance()->GetStageData(stageName_);

	state_ = EditorState::Edit;
}

void StageEditor::WriteStageFile() {
	//データ入力
	stageData_.bossData.filepath = bossData_.filePath;
	stageData_.bossData.spawnPosition = bossData_.spawnPosition;

	stageData_.playerSpawnPosition = playerSpownPosition_;

	stageData_.colliderObjects.clear();
	for (auto& object : colliderObjects_) {
		//無効化されているオブジェクトは使用しない
		if (object.enableObject)continue;

		//一旦衝突判定無し
		ColliderObjectData data;
		data.directoryPath = object.directoryPath;
		data.filename = object.filename;
		data.transform = object.transform;
		stageData_.colliderObjects.push_back(data);
	}

	StageManager::GetInstance()->WriteStage(stageName_, stageData_);
}

void StageEditor::AddColliderObject(int32_t index, std::string directoryPath, std::string filename, SRT transform) {
	//新しく作った場合のみobjectを追加
	if (colliderObjects_.size() <= index) {
		EditorObject editorObject;
		editorObject.object = std::make_shared<Object>();
		editorObject.object->Initialize(ModelManager::GetInstance()->GetModel(directoryPath, filename));

		colliderObjects_.push_back(std::move(editorObject));
	}

	colliderObjects_[index].directoryPath = directoryPath;
	colliderObjects_[index].filename = filename;
	colliderObjects_[index].transform = transform;
	colliderObjects_[index].enableObject = true;
}

void StageEditor::DeleteGroundObject(int32_t index) {
	assert(colliderObjects_.size() <= index);
	colliderObjects_[index].enableObject = false;
}

void StageEditor::SetGroundObjectPosition(int32_t index, Vector3 position) {
	assert(colliderObjects_.size() <= index);
	colliderObjects_[index].transform.translate = position;
}

void StageEditor::SetGroundObjectDirection(int32_t index, Vector3 direction) {
	assert(colliderObjects_.size() <= index);
	colliderObjects_[index].transform.rotate = direction;	//後でクォータニオンにしよう
}

void StageEditor::SetGroundObjectScale(int32_t index, Vector3 scale) {
	assert(colliderObjects_.size() <= index);
	colliderObjects_[index].transform.scale = scale;
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
						AddColliderObject(currentColliderIndex_, path, entry.path().filename().string() , { {1,1,1},{0,0,0},{0,0,0} });
						currentColliderIndex_++;
					}
				}
			}
		}
		ImGui::TreePop();
	}
}