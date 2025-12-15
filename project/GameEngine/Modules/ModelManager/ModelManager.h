#pragma once
#include <string>
#include <wrl.h>
#include <DirectXCommon/DirectXCommon.h>
#include <Model/Model.h>

using namespace std;

class ModelManager {
private:

	static unique_ptr<ModelManager> instance;

	DirectXCommon* dxCommon_ = nullptr;

	//テクスチャデータ
	std::unordered_map <std::string,shared_ptr<Model>> modelDatas;
public:

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator=(ModelManager&) = delete;

	//シングルトンインスタンスの取得
	static ModelManager* GetInstance();

	//終了
	void Finalize();

	//初期化
	void Initialize(DirectXCommon* dxCommon);

	//objファイルの読み込み
	void LoadModel(const std::string& directoryPath, const std::string& filename);

	//モデルの入手
	shared_ptr<Model> GetModel(const std::string& directoryPath, const std::string& filename);
};