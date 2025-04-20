#pragma once
#include <wrl.h>
#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>
#include <cstdint>

Microsoft::WRL::ComPtr <ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr <ID3D12Device> device, int32_t width, int32_t height);