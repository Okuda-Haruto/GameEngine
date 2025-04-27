#pragma once

#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>

#include <wrl.h>
#include <format>

//テクスチャ
class Texture {
private:

	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;

public:
	
	void Initialize(const std::string& filePath,
		Microsoft::WRL::ComPtr <ID3D12Device> device,
		Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList,
		Microsoft::WRL::ComPtr <ID3D12DescriptorHeap>& srvDescriptorHeap,
		uint32_t descriptorSizeSRV,
		uint32_t& CPUIndex,
		uint32_t& GPUIndex);

	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU();
};