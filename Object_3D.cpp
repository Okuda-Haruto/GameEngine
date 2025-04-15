#include "Object_3D.h"

/*Object_3D::Object_3D(Microsoft::WRL::ComPtr<ID3D12Device>& device) {
	//頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(device, sizeof(VertexData) * modelData_.vertices.size());
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();	//リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());	//使用するリソースのサイズは頂点のサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);	//1頂点あたりのサイズ

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));	//書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());	//頂点データにリソースにコピー

	//マテリアル用のリソースを作る。
	materialResource_ = CreateBufferResource(device, sizeof(Material));
	//マテリアルにデータを書き込む
	materialData_ = nullptr;
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//マテリアルの色を入力する
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	materialData_->uvTransform = MakeIdentity4x4();
	//Lightingを有効化する
	materialData_->enableLighting = true;
}*/