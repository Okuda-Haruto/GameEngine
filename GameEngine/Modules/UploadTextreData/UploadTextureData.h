#pragma once
#include <wrl.h>
#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>
#include "DirectXTex/DirectXTex.h"

[[nodiscard]]
Microsoft::WRL::ComPtr <ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr <ID3D12Resource> texture,
	const DirectX::ScratchImage& mipImages,
	Microsoft::WRL::ComPtr <ID3D12Device> device,
	Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList);