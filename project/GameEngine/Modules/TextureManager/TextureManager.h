#pragma once
#include <string>
#include <vector>
#include "DirectXTex/DirectXTex.h"
#include <wrl.h>
#include <d3d12.h>
#include <DirectXCommon/DirectXCommon.h>
#include <SRVManager/SRVManager.h>

using namespace std;

class TextureManager {
private:
	static unique_ptr<TextureManager> instance;

	DirectXCommon* dxCommon_ = nullptr;
	SRVManager* srvManager_ = nullptr;

	//SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;

	//テクスチャ一枚分のデータ
	struct TextureData {
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		int32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};
	//テクスチャデータ
	std::unordered_map<std::string,TextureData> textureDatas;

	//white2x2のsrvIndex
	int32_t white2x2;

public:

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

	//シングルトンインスタンスの取得
	static TextureManager* GetInstance();

	//終了
	void Finalize();

	//初期化
	void Initialize(DirectXCommon* dxCommon, SRVManager* srvManager);

	//テクスチャファイルの読み込み
	void LoadTexture(const std::string& filePath);

	//メタデータを取得
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);
	//SRVインデクスの取得
	uint32_t GetSrvIndex(const std::string& filePath);
	//GPUハンドルの取得
	D3D12_GPU_DESCRIPTOR_HANDLE  GetSrvHandleGPU(const std::string& filePath);

	int32_t GetWhite2x2() { return white2x2; }
};