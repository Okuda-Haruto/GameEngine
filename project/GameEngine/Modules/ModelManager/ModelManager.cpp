#include "ModelManager.h"
#include <algorithm>
#include "ConvertString.h"

std::unique_ptr<ModelManager> ModelManager::instance;

ModelManager* ModelManager::GetInstance() {
	if (!instance) {
		instance = std::make_unique<ModelManager>();
	}
	return instance.get();
}

void ModelManager::Finalize() {
	for (auto& data : modelDatas) {
		data.second.reset();
	}
	modelDatas.clear();

	instance.reset();
}

void ModelManager::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
}

//テクスチャファイルの読み込み
void ModelManager::LoadModel(const std::string& directoryPath, const std::string& filename) {

	if (modelDatas.contains(directoryPath + "/" + filename)) {
		return;
	}

	std::shared_ptr<Model>& model = modelDatas[directoryPath + "/" + filename];

	model = std::make_shared<Model>();
	model->Initialize(directoryPath, filename, dxCommon_);
}

//モデルの入手
const std::weak_ptr<Model> ModelManager::GetModel(const std::string& directoryPath, const std::string& filename) {
	
	if (modelDatas.contains(directoryPath + "/" + filename)) {
		return modelDatas[directoryPath + "/" + filename];
	}

	//モデルがないならもう一度
	LoadModel(directoryPath, filename);

	return modelDatas[directoryPath + "/" + filename];
}