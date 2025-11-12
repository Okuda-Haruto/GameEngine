#include "ModelHolder.h"
#include <ModelManager/ModelManager.h>

ModelHolder::~ModelHolder() {
	
}

void ModelHolder::Initialize() {
	FilePath filePath;
	filePath.directoryPath_ = "resources/BackGround/Skydome";
	filePath.fileName_ = "BackGround.obj";
	filePathes_.push_back(filePath);
	filePath.directoryPath_ = "resources/BackGround/Ground";
	filePath.fileName_ = "Ground.obj";
	filePathes_.push_back(filePath);
	filePath.directoryPath_ = "resources/Caracter/Player";
	filePath.fileName_ = "Player.obj";
	filePathes_.push_back(filePath);

	for (FilePath filePath : filePathes_) {
		ModelManager::GetInstance()->LoadModel(filePath.directoryPath_, filePath.fileName_);
	}
	
}

Model* ModelHolder::GetModel(ModelIndex model) {
	return ModelManager::GetInstance()->GetModel(filePathes_[int32_t(model)].directoryPath_, filePathes_[int32_t(model)].fileName_);
}