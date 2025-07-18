#pragma once
#include <string>
#include <wrl.h>
#include <d3d12.h>

struct TextureData {
	//テクスチャへのパス
	std::string tetxureFilePaths;
	//テクスチャのリソース
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResource;
};