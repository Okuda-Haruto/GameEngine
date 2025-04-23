#include "Object_3D.h"

#include "CreateBufferResource.h"
#include "LoadObjFile.h"
#include "GetDescriptorHandle.h"
#include "LoadTexture.h"
#include "CreateTextureResource.h"
#include "UploadTextureData.h"

void Object_3D::Initialize(const std::string& directoryPath, const std::string& filename, Microsoft::WRL::ComPtr<ID3D12Device>& device) {
	//モデル読み込み
	modelData_ = LoadObjFile(directoryPath, filename);
	//頂点リソースを作る
	vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * modelData_.vertices.size());
	//頂点バッファビューを作成する
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();	//リソースの先頭のアドレスから使う
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());	//使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);	//1頂点あたりのサイズ

	//頂点リソースにデータを書き込む
	vertexData_ = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));	//書き込むためのアドレスを取得
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());	//頂点データにリソースにコピー

	//マテリアル用のリソースを作る。
	materialResource_ = CreateBufferResource(device, sizeof(Material));
	//マテリアルにデータを書き込む
	materialData_ = nullptr;
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//マテリアルの色を入力する
	materialData_->color = Vector4(1.0f, 0.0f, 1.0f, 1.0f);

	materialData_->uvTransform = MakeIdentity4x4();
	//Lightingを有効化する
	materialData_->enableLighting = true;



	//WVP用のリソースを作る
	wvpResource_ = CreateBufferResource(device, sizeof(TransformationMatrix));
	//データを書き込む
	wvpData_ = nullptr;
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列を書き込んでおく
	wvpData_->WVP = MakeIdentity4x4();

}

void Object_3D::Draw(Transform& transform, Vector4 color,Transform& cameraTransform, Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& commandList, Microsoft::WRL::ComPtr<ID3D12Resource>& directionalLightResource, D3D12_GPU_DESCRIPTOR_HANDLE& textureSrvHandleGPU) {
	materialData_->color = color;
	
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	wvpData_->World = worldMatrix;
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	wvpData_->WVP = worldViewProjectionMatrix;

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());	//Lighting

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//描画(DrawCall)
	//commandList->DrawIndexedInstanced(kSubdivision* kSubdivision * 6, 1, 0, 0, 0);
	commandList->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
}

ModelData Object_3D::ModelData() {
	return modelData_;
}