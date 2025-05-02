#pragma once

#include "externals/DirectXTex/DirectXTex.h"
#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>

#include <wrl.h>
#include <format>

//テクスチャ
class Texture {
private:

	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;

	Microsoft::WRL::ComPtr <ID3D12Resource> textureResource_;

public:

	void Initialize(const std::string& filePath,
		Microsoft::WRL::ComPtr <ID3D12Device> device,
		Microsoft::WRL::ComPtr <ID3D12CommandQueue>& commandQueue,
		Microsoft::WRL::ComPtr <ID3D12CommandAllocator>& commandAllocator,
		Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& commandList,
		Microsoft::WRL::ComPtr <ID3D12Fence>& fence,
		uint64_t& fenceValue,
		HANDLE& fenceEvent,
		Microsoft::WRL::ComPtr <ID3D12DescriptorHeap>& srvDescriptorHeap,
		uint32_t descriptorSizeSRV,
		uint32_t& CPUIndex,
		uint32_t& GPUIndex);

	[[nodiscard]]
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU();
};