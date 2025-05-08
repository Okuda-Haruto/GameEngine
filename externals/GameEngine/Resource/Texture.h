#pragma once

#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>

#include <wrl.h>
#include <format>

//テクスチャ
class Texture {
private:

	//テクスチャのGPUデスクリプタハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
	//テクスチャリソース
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResource_;

public:

	// 初期化	コマンドリストを使用するために必要な物はGameEngineから引っ張ってくること
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

	//テクスチャのGPUデスクリプタハンドル
	[[nodiscard]]
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU();
};