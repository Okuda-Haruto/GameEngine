#include "ModelHolder.h"
#include <ModelManager/ModelManager.h>

ModelHolder::~ModelHolder() {
	
}

void ModelHolder::Initialize() {
	FilePath filePath;
	//天球
	filePath.directoryPath_ = "resources/BackGround/Skydome";
	filePath.fileName_ = "Skydome.obj";
	filePathes_.push_back(filePath);
	//地面
	filePath.directoryPath_ = "resources/BackGround/Ground";
	filePath.fileName_ = "Ground.obj";
	filePathes_.push_back(filePath);
	//柵
	filePath.directoryPath_ = "resources/BackGround/Fence";
	filePath.fileName_ = "Fence.obj";
	filePathes_.push_back(filePath);
	//プレイヤー
	filePath.directoryPath_ = "resources/Caracter/Player";
	filePath.fileName_ = "Player.obj";
	filePathes_.push_back(filePath);
	//ボス
	filePath.directoryPath_ = "resources/Caracter/Boss";
	filePath.fileName_ = "Boss.obj";
	filePathes_.push_back(filePath);
	//弾丸
	filePath.directoryPath_ = "resources/Bullet";
	filePath.fileName_ = "Bullet.obj";
	filePathes_.push_back(filePath);
	//シリンダー
	filePath.directoryPath_ = "resources/Cylinder";
	filePath.fileName_ = "Cylinder.obj";
	filePathes_.push_back(filePath);
	//ハット(HP)
	filePath.directoryPath_ = "resources/Hat";
	filePath.fileName_ = "Hat.obj";
	filePathes_.push_back(filePath);

	for (FilePath filePath : filePathes_) {
		ModelManager::GetInstance()->LoadModel(filePath.directoryPath_, filePath.fileName_);
	}
	
}

Model* ModelHolder::GetModel(ModelIndex model) {
	return ModelManager::GetInstance()->GetModel(filePathes_[int32_t(model)].directoryPath_, filePathes_[int32_t(model)].fileName_);
}