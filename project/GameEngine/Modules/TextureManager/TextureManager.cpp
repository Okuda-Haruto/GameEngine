#include "TextureManager.h"
#include <algorithm>
#include "ConvertString.h"

uint32_t TextureManager::kSRVIndexTop = 1;
unique_ptr<TextureManager> TextureManager::instance;

TextureManager* TextureManager::GetInstance() {
	if (!instance) {
		instance = make_unique<TextureManager>();
	}
	return instance.get();
}

void TextureManager::Finalize() {
	instance.reset();
}

void TextureManager::Initialize(DirectXCommon* dxCommon, SRVManager* srvManager) {
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	//テクスチャ初期値としてwhite2x2を読み込む
	white2x2 = GetSrvIndex("resources/DebugResources/white2x2.png");
}

//テクスチャファイルの読み込み
void TextureManager::LoadTexture(const std::string& filePath) {

	//読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) {
		//読み込み済みなら早期Return
		return;
	}

	//テクスチャ上限チェック
	assert(srvManager_->IsCanAllocate());

	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//テクスチャデータを追加して書き込む
	TextureData& textureData = textureDatas[filePath];

	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);

	dxCommon_->UploadTextureData(textureData.resource.Get(), mipImages);

	textureData.srvIndex = srvManager_->Allocate();
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

	//SRV生成
	srvManager_->CreateSRVforTexture2D(textureData.srvIndex, textureData.resource.Get(), textureData.metadata.format, UINT(textureData.metadata.mipLevels));
}

uint32_t TextureManager::GetSrvIndex(const std::string& filePath) {
	//読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) {
		//読み込み済みなら要素番号を返す
		uint32_t textureIndex = textureDatas[filePath].srvIndex;
		return textureIndex;
	}

	//読み込んでいないテクスチャを指定した場合ここで読み込む
	LoadTexture(filePath);

	//読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) {
		//読み込み済みなら要素番号を返す
		uint32_t textureIndex = textureDatas[filePath].srvIndex;
		return textureIndex;
	}

	assert(0);
	return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath) {
	TextureData& textureData = textureDatas[filePath];
	//読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) {
		return  textureData.srvHandleGPU;
	}
	assert(0);
	return  textureData.srvHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath) {
	TextureData& textureData = textureDatas[filePath];
	//読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) {
		return  textureData.metadata;
	}
	assert(0);
	return  textureData.metadata;
}