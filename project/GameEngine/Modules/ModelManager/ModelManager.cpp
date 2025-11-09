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
	delete instance;
	instance = nullptr;
}

void ModelManager::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
}

//テクスチャファイルの読み込み
void ModelManager::LoadModel(const std::string& directoryPath, const std::string& filename) {

	Model* model = new Model;
	model->Initialize(directoryPath, filename, dxCommon_);
	modelDatas.push_back(model);
}