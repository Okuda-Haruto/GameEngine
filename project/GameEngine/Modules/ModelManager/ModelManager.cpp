#include "ModelManager.h"
#include <algorithm>
#include "ConvertString.h"

ModelManager* ModelManager::instance = nullptr;

ModelManager* ModelManager::GetInstance() {
	if (instance == nullptr) {
		instance = new ModelManager;
	}
	return instance;
}

void ModelManager::Finalize() {
	for (auto& data : modelDatas) {
		delete data.second;
		data.second = nullptr;
	}
	modelDatas.clear();

	delete instance;
	instance = nullptr;
}

void ModelManager::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
}

//テクスチャファイルの読み込み
void ModelManager::LoadModel(const std::string& directoryPath, const std::string& filename) {

	if (modelDatas.contains(directoryPath + "/" + filename)) {
		return;
	}

	Model*& model = modelDatas[directoryPath + "/" + filename];

	model = new Model;
	model->Initialize(directoryPath, filename, dxCommon_);
}

//モデルの入手
Model* ModelManager::GetModel(const std::string& directoryPath, const std::string& filename) {
	
	if (modelDatas.contains(directoryPath + "/" + filename)) {
		return modelDatas[directoryPath + "/" + filename];
	}

	//モデルがないならもう一度
	LoadModel(directoryPath, filename);

	return modelDatas[directoryPath + "/" + filename];
}