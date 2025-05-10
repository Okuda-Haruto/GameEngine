#include "Object_3D.h"

#include "Matrix4x4_operation.h"

#include "CreateBufferResource.h"
#include "LoadObjFile.h"
#include "GetDescriptorHandle.h"
#include "LoadTexture.h"
#include "CreateTextureResource.h"
#include "UploadTextureData.h"

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

	//使用するリソースのサイズを0にしておく
	wvpResource_.resize(0);
	wvpData_.resize(0);
	materialResource_.resize(0);
	materialData_.resize(0);
}

void Object_3D::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Object_3D_Data& data) {

	//空の要素を追加
	wvpResource_.emplace_back();
	wvpData_.emplace_back();
	materialResource_.emplace_back();
	materialData_.emplace_back();

	//初期化
	wvpResource_.back() = CreateBufferResource(device_, sizeof(TransformationMatrix));
	wvpData_.back() = nullptr;
	materialResource_.back() = CreateBufferResource(device_, sizeof(Material));
	materialData_.back() = nullptr;

	//WVPデータを更新
	wvpResource_.back()->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_.back()));

	Matrix4x4 worldMatrix = MakeAffineMatrix(data.transform.scale, data.transform.rotate, data.transform.translate);
	wvpData_.back()->World = worldMatrix;
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(data.camera.viewMatrix, data.camera.projectionMatrix));
	wvpData_.back()->WVP = worldViewProjectionMatrix;

	wvpResource_.back()->Unmap(0, nullptr);

	//マテリアルデータを更新
	materialResource_.back()->Map(0, nullptr, reinterpret_cast<void**>(&materialData_.back()));

	materialData_.back()->color = data.color;
	materialData_.back()->uvTransform = MakeAffineMatrix(data.uvTransform.scale, data.uvTransform.rotate, data.uvTransform.translate);
	materialData_.back()->enableLighting = isLighting_;

	materialResource_.back()->Unmap(0, nullptr);

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList->SetGraphicsRootDescriptorTable(2, texture_->textureSrvHandleGPU());
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定
	if (isLighting_) {
		commandList->SetGraphicsRootConstantBufferView(3, light_->directionalLightResource()->GetGPUVirtualAddress());	//Lighting
	}

	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_.back()->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_.back()->GetGPUVirtualAddress());
	//描画(DrawCall)
	//commandList->DrawIndexedInstanced(kSubdivision* kSubdivision * 6, 1, 0, 0, 0);
	commandList->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
}

void Object_3D::Reset() {
	//使用するリソースのサイズを0にしておく
	wvpResource_.resize(0);
	wvpData_.resize(0);
	materialResource_.resize(0);
	materialData_.resize(0);
}

[[nodiscard]]
ModelData Object_3D::ModelData() { return modelData_; }