#include "Texture.h"

#include "externals/DirectXTex/DirectXTex.h"
#include "LoadTexture.h"
#include "CreateTextureResource.h"
#include "UploadTextureData.h"
#include "GetDescriptorHandle.h"

void Texture::Initialize(const std::string& filePath,
	Microsoft::WRL::ComPtr <ID3D12Device> device,
	Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList,
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap>& srvDescriptorHeap,
	uint32_t descriptorSizeSRV,
	uint32_t& CPUIndex,
	uint32_t& GPUIndex)
{
	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTexture(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResource(device, metadata);
	Microsoft::WRL::ComPtr <ID3D12Resource> intermediateResource = UploadTextureData(textureResource.Get(), mipImages, device.Get(), commandList.Get());

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
	device->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

	CPUIndex++;
	GPUIndex++;
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture::textureSrvHandleGPU() { return textureSrvHandleGPU_; }