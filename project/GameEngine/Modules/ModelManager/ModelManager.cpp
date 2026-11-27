#include "ModelManager.h"
#include <algorithm>
#include "ConvertString.h"

void ModelManager::Finalize() {
	for (auto& data : modelDatas_) {
		data.second.reset();
	}
	modelDatas_.clear();
}

void ModelManager::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
}

//テクスチャファイルの読み込み
void ModelManager::LoadModel(const std::string& directoryPath, const std::string& fileName) {

	if (modelDatas_.contains(directoryPath + "/" + fileName)) {
		return;
	}

	std::shared_ptr<Model>& model = modelDatas_[directoryPath + "/" + fileName];

	model = std::make_shared<Model>();
	model->Initialize(directoryPath, fileName, dxCommon_);
}

//モデルの入手
const std::weak_ptr<Model> ModelManager::GetModel(const std::string& directoryPath, const std::string& fileName) {
	
	if (modelDatas_.contains(directoryPath + "/" + fileName)) {
		return modelDatas_[directoryPath + "/" + fileName];
	}

	//モデルがないならもう一度
	LoadModel(directoryPath, fileName);

	return modelDatas_[directoryPath + "/" + fileName];
}