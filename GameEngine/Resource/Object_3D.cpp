#include "Object_3D.h"

#include "Matrix4x4_operation.h"

#include "CreateBufferResource.h"
#include "LoadObjFile.h"
#include "GetDescriptorHandle.h"
#include "LoadTexture.h"
#include "CreateTextureResource.h"
#include "UploadTextureData.h"

Object_3D::~Object_3D() {
	wvpResource_.clear();
	wvpData_.clear();
	materialResource_.clear();
	materialData_.clear();
	vertexData_ = nullptr;
}

void Object_3D::Initialize(const std::string& directoryPath, const std::string& filename, Microsoft::WRL::ComPtr<ID3D12Device> device) {

	device_ = device;

	//モデル読み込み
	modelData_ = LoadObjFile(directoryPath, filename);

	//頂点リソースを作る
	vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * modelData_.vertices.size());

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));	//書き込むためのアドレスを取得

	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());	//頂点データにリソースにコピー

	vertexResource_->Unmap(0, nullptr);

	//使用するリソースの要素を予め用意する
	wvpResource_.resize(kMaxIndex);
	wvpData_.resize(kMaxIndex);
	materialResource_.resize(kMaxIndex);
	materialData_.resize(kMaxIndex);

	//初期化
	for (int i = 0; i < kMaxIndex; i++) {
		wvpResource_[i] = CreateBufferResource(device_, sizeof(TransformationMatrix));
		wvpData_[i] = nullptr;
		materialResource_[i] = CreateBufferResource(device_, sizeof(Material));
		materialData_[i] = nullptr;
	}

	//最初から始める
	index_ = 0;
}

void Object_3D::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Object_3D_Data& data) {

	//WVPデータを更新
	wvpResource_[index_]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_[index_]));

	Matrix4x4 worldMatrix = MakeAffineMatrix(data.transform.scale, data.transform.rotate, data.transform.translate);
	wvpData_[index_]->World = worldMatrix;
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(data.camera.viewMatrix, data.camera.projectionMatrix));
	wvpData_[index_]->WVP = worldViewProjectionMatrix;

	wvpResource_[index_]->Unmap(0, nullptr);

	//マテリアルデータを更新
	materialResource_[index_]->Map(0, nullptr, reinterpret_cast<void**>(&materialData_[index_]));

	materialData_[index_]->color = data.color;
	materialData_[index_]->uvTransform = MakeAffineMatrix(data.uvTransform.scale, data.uvTransform.rotate, data.uvTransform.translate);
	materialData_[index_]->enableLighting = isLighting_;

	materialResource_[index_]->Unmap(0, nullptr);

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList->SetGraphicsRootDescriptorTable(2, texture_->textureSrvHandleGPU());
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定
	if (isLighting_) {
		commandList->SetGraphicsRootConstantBufferView(3, light_->directionalLightResource()->GetGPUVirtualAddress());	//Lighting
	}

	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_[index_]->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_[index_]->GetGPUVirtualAddress());
	//描画(DrawCall)
	//commandList->DrawIndexedInstanced(kSubdivision* kSubdivision * 6, 1, 0, 0, 0);
	commandList->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

	//最大値を超えたら最初から
	index_++;
	if (index_ >= kMaxIndex) {
		index_ = 0;
	}
}

void Object_3D::Reset() {
	//使用するリソースの要素を空にしておく
	wvpResource_.clear();
	wvpData_.clear();
	materialResource_.clear();
	materialData_.clear();
}

[[nodiscard]]
ModelData Object_3D::ModelData() { return modelData_; }