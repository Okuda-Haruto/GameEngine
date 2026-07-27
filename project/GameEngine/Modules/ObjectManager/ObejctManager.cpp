#include "ObejctManager.h"

void ObjectManager::Initialize(DirectXCommon* dxCommon, SRVManager* srvManager) {

	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	nextVertexAllocation = 0;
	nextIndexAllocation = 0;

	//頂点リソースを作る
	vertexResource_ = dxCommon_->CreateOutputResources(sizeof(VertexData) * maxIndexSize);

	verticesBufferSRVindex_ = srvManager_->Allocate();
	verticesBufferUAVindex_ = srvManager_->Allocate();
	srvManager_->CreateSRVforStructuredBuffer(verticesBufferSRVindex_, vertexResource_.Get(), maxIndexSize, sizeof(VertexData));
	srvManager_->CreateUAVforStructuredBuffer(verticesBufferUAVindex_, vertexResource_.Get(), maxIndexSize, sizeof(VertexData));

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * maxIndexSize);
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//Sprite用のインデックスリソースを作る
	indexResource_ = dxCommon_->CreateBufferResources(sizeof(uint32_t) * maxIndexSize);

	//インデックスバッファビューを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックスのサイズ
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * maxIndexSize);
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}

OffsetAllocation ObjectManager::MakeNewOffsetAllocation(shared_ptr<Model> model) {

	std::vector<VertexData> vertices = model->GetVertices();
	std::vector<uint32_t> indexes = model->GetIndexes();

	OffsetAllocation allocation;
	allocation.vertexStart = nextVertexAllocation;
	allocation.indexStart = nextIndexAllocation;
	allocation.vertexCount = uint32_t(vertices.size());
	allocation.indexCount = uint32_t(indexes.size());

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertexData_));

	memcpy(mappedVertexData_ + allocation.vertexStart, vertices.data(), vertices.size() * sizeof(VertexData));

	vertexResource_->Unmap(0, nullptr);

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndexData_));

	memcpy(mappedIndexData_ + allocation.indexStart, indexes.data(), indexes.size() * sizeof(uint32_t));

	indexResource_->Unmap(0, nullptr);

	nextVertexAllocation += allocation.vertexCount;
	nextIndexAllocation += allocation.indexCount;

	return allocation;
}

Microsoft::WRL::ComPtr<ID3D12Resource> ObjectManager::MakeObjectDataSRVResource() {
	return dxCommon_->CreateBufferResources(sizeof(ObjectData));
}

Microsoft::WRL::ComPtr<ID3D12Resource> ObjectManager::MakeObjectDataUAVResource(){
	return dxCommon_->CreateOutputResources(sizeof(ObjectData));
}