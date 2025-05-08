#pragma once
#include <wrl.h>
#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>
#include "DirectXTex/DirectXTex.h"

ID3D12Resource* CreateTextureResource(Microsoft::WRL::ComPtr <ID3D12Device> device, const DirectX::TexMetadata& metadata);