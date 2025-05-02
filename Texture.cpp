#include "Texture.h"

#include "externals/DirectXTex/DirectXTex.h"
#include "LoadTexture.h"
#include "CreateTextureResource.h"
#include "UploadTextureData.h"
#include "GetDescriptorHandle.h"

void Texture::Initialize(const std::string& filePath,
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
	uint32_t& GPUIndex)
{
	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTexture(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	textureResource_ = CreateTextureResource(device, metadata); 
	Microsoft::WRL::ComPtr <ID3D12Resource> intermediateResource = UploadTextureData(textureResource_.Get(), mipImages, device.Get(), commandList.Get());
	//コマンドリストの内容を確定させる。すべてのコマンドを詰んでからCloseすること
	HRESULT hr = commandList->Close();
	assert(SUCCEEDED(hr));

	//GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(1, commandLists);

	//Fenceの値を更新
	fenceValue++;
	//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue->Signal(fence.Get(), fenceValue);

	//Fenceの値が指定したSignal値に辿り着いているか確認する
	//GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence->GetCompletedValue() < fenceValue) {
		//指定したSignalに辿り着いていないので、辿り着くまで待つようにイベントを設定する
		fence->SetEventOnCompletion(fenceValue, fenceEvent);
		//イベントを待つ
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	//次のフレーム用のコマンドリストを準備
	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator.Get(), nullptr);
	assert(SUCCEEDED(hr));

	intermediateResource->Release();

	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める。ImGuiが最初を使うのでその次を使う
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, CPUIndex);
	textureSrvHandleGPU_ = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, GPUIndex);
	//SRVの生成
	device->CreateShaderResourceView(textureResource_.Get(), &srvDesc, textureSrvHandleCPU);

	CPUIndex++;
	GPUIndex++;
}

[[nodiscard]]
D3D12_GPU_DESCRIPTOR_HANDLE Texture::textureSrvHandleGPU() { return textureSrvHandleGPU_; }