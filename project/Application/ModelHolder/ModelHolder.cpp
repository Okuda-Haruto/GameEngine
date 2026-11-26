#include "ModelHolder.h"
#include "ModelManager/ModelManager.h"

std::unique_ptr<ModelHolder> ModelHolder::instance;

ModelHolder* ModelHolder::GetInstance() {
	if (!instance) {
		instance = std::make_unique<ModelHolder>();
	}
	return instance.get();
}

void ModelHolder::Finalize() {
	instance.reset();
}

void ModelHolder::Initialize() {
	FilePath filePath;
	//天球
	filePath.directoryPath_ = "resources/BackGround/Skydome";
	filePath.fileName_ = "Skydome.obj";
	filePaths_.push_back(filePath);
	//地面
	filePath.directoryPath_ = "resources/BackGround/Ground";
	filePath.fileName_ = "Ground.obj";
	filePaths_.push_back(filePath);
	//柵
	filePath.directoryPath_ = "resources/BackGround/Fence";
	filePath.fileName_ = "Fence.obj";
	filePaths_.push_back(filePath);
	//背景オブジェクト
	filePath.directoryPath_ = "resources/BackGround/BackGround";
	filePath.fileName_ = "BackGround.obj";
	filePaths_.push_back(filePath);
	//タンブルウィード
	filePath.directoryPath_ = "resources/BackGround/Tumbleweed";
	filePath.fileName_ = "Tumbleweed.obj";
	filePaths_.push_back(filePath);
	//プレイヤー
	filePath.directoryPath_ = "resources/Character/Player";
	filePath.fileName_ = "Player.obj";
	filePaths_.push_back(filePath);
	//ボス
	filePath.directoryPath_ = "resources/Character/Boss";
	filePath.fileName_ = "Boss_1_anm.gltf";
	filePaths_.push_back(filePath);
	//弾丸
	filePath.directoryPath_ = "resources/Bullet";
	filePath.fileName_ = "Bullet.obj";
	filePaths_.push_back(filePath);
	//シリンダー
	filePath.directoryPath_ = "resources/HUD/Cylinder";
	filePath.fileName_ = "Cylinder.obj";
	filePaths_.push_back(filePath);
	//ハット(HP)
	filePath.directoryPath_ = "resources/HUD/Hat";
	filePath.fileName_ = "Hat.obj";
	filePaths_.push_back(filePath);

	for (FilePath filePath : filePaths_) {
		ModelManager::GetInstance()->LoadModel(filePath.directoryPath_, filePath.fileName_);
	}
	
}

const std::weak_ptr<Model> ModelHolder::GetModel(ModelIndex model) {
	return ModelManager::GetInstance()->GetModel(filePaths_[int32_t(model)].directoryPath_, filePaths_[int32_t(model)].fileName_);
}