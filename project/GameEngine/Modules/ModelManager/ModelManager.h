#pragma once
#include <string>
#include <wrl.h>
#include <DirectXCommon/DirectXCommon.h>
#include <Model/Model.h>

class ModelManager {
private:
	static ModelManager* instance;

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator=(ModelManager&) = delete;

	DirectXCommon* dxCommon_ = nullptr;

	//テクスチャデータ
	std::vector<Model*> modelDatas;
public:
	//シングルトンインスタンスの取得
	static ModelManager* GetInstance();

	//終了
	void Finalize();

	//初期化
	void Initialize(DirectXCommon* dxCommon);

	//objファイルの読み込み
	void LoadModel(const std::string& directoryPath, const std::string& filename);

	//モデルの入手
	Model* GetModel(UINT modelIndex) { assert(modelIndex <= modelDatas.size()); return modelDatas[modelIndex]; }
};