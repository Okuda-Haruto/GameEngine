#pragma once
#include <string>
#include <vector>
#include "DirectXTex/DirectXTex.h"
#include <wrl.h>
#include <d3d12.h>
#include <DirectXCommon/DirectXCommon.h>

class TextureManager {
private:
	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

	DirectXCommon* dxCommon_ = nullptr;

	//SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;

	//テクスチャ一枚分のデータ
	struct TextureData {
		std::string filePath;
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};
	//テクスチャデータ
	std::vector<TextureData> textureDatas;
public:
	//シングルトンインスタンスの取得
	static TextureManager* GetInstance();

	//終了
	void Finalize();

	//初期化
	void Initialize(DirectXCommon* dxCommon);

	//テクスチャファイルの読み込み
	void LoadTexture(const std::string& filePath);

	//SRVインデックスの開始番号
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);
	//テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);
	//メタデータを取得
	const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex);
};