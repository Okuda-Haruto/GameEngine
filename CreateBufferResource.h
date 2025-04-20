#pragma once
#include <wrl.h>
#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>

// リソース関数
Microsoft::WRL::ComPtr <ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr <ID3D12Device> device, size_t sizeInBytes);