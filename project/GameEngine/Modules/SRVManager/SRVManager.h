#pragma once
#include <string>
#include <vector>
#include "DirectXTex/DirectXTex.h"
#include <wrl.h>
#include <d3d12.h>
#include <DirectXCommon/DirectXCommon.h>

class SRVManager {
private:
	//最大SRV数(最大テクスチャ枚数)
	static const uint32_t kMaxSRVCount;
	//SRV用のデスクリプタサイズ
	uint32_t descriptorSize_;
	//SRV用のヒープディスクリプタ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descriptorHeap_;

	DirectXCommon* dxCommon_ = nullptr;

	uint32_t StructuredBufferIndex_;

	UINT useIndex = 0;
public:

	~SRVManager();

	//初期化
	void Initialize(DirectXCommon* dxCommon);

	//デスクリプタ確保
	uint32_t Allocate();

	//指定番号のCPUデスクリプタハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	//指定番号のGPUデスクリプタハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

	//SRV生成(テクスチャ用)
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	//SRV生成(Structured Buffer用)
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElement, UINT structureByteStride);
	//SRV生成(深度バッファ用)
	void CreateSRVforDepthBuffer(uint32_t srvIndex, ID3D12Resource* pResource);

	//ヒープをセット
	void PreDraw();

	//SRVセット
	void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);

	bool IsCanAllocate();

	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> GetDescriptorHeap() { return descriptorHeap_; }
};