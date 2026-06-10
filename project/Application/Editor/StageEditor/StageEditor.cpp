#include "StageEditor.h"
#include <cassert>

void StageEditor::Initialize() {
	commandIndex_ = 0;

	bossData_.spownPosition = {};
	playerSpownPosition_ = { 0.0f,0.0f,-20.0f };
}

void StageEditor::Update() {

}

void StageEditor::Draw() {

}

void StageEditor::ReadStageFile(std::string filePath) {

}

void StageEditor::WriteStageFile(std::string filePath) {

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
	bossData_.spownPosition = spownPosition;
}

void StageEditor::SetPlayerSpownPosition(Vector3 spownPosition) {
	playerSpownPosition_ = spownPosition;
}