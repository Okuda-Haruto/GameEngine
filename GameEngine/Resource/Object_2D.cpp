#include "Object_2D.h"

#include "Matrix4x4_operation.h"

#include "CreateBufferResource.h"

Object_2D::~Object_2D() {
	vertexResource_->Unmap(0, nullptr);
	vertexData_ = nullptr;
	wvpData_.clear();
	materialData_.clear();
	delete indexData_;

	delete texture_;
}

void Object_2D::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t kWindowWidth, uint32_t kWindowHeight) {

	device_ = device;

	kWindowWidth_ = kWindowWidth;
	kWindowHeight_ = kWindowHeight;

	//Sprite用の頂点リソースを作る
	vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * 4);

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点は4つサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点のローカル座標系を設定
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//左下
	vertexData_[0].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[0].normal = { 0.0f,0.0f,-1.0f };
	//左上
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[1].normal = { 0.0f,0.0f,-1.0f };
	//右下
	vertexData_[2].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[2].normal = { 0.0f,0.0f,-1.0f };
	//右上
	vertexData_[3].position = { 640.0f,0.0f,0.0f,1.0f };
	vertexData_[3].texcoord = { 1.0f,0.0f };
	vertexData_[3].normal = { 0.0f,0.0f,-1.0f };



	//Sprite用のインデックスリソースを作る
	indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * 6);

	//インデックスバッファビューを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックスは6つサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	//3角形2つを組合わせ4角形にする
	indexData_[0] = 0; indexData_[1] = 1; indexData_[2] = 2;
	indexData_[3] = 1; indexData_[4] = 3; indexData_[5] = 2;

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

void Object_2D::Draw(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& commandList, Object_2D_Data& data) {

	//左下
	vertexData_[0].position = { 0.0f,spriteWidth_,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[0].normal = { 0.0f,0.0f,-1.0f };
	//左上
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[1].normal = { 0.0f,0.0f,-1.0f };
	//右下
	vertexData_[2].position = { spriteHeight_,spriteWidth_,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[2].normal = { 0.0f,0.0f,-1.0f };
	//右上
	vertexData_[3].position = { spriteHeight_,0.0f,0.0f,1.0f };
	vertexData_[3].texcoord = { 1.0f,0.0f };
	vertexData_[3].normal = { 0.0f,0.0f,-1.0f };

	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowWidth_), float(kWindowHeight_), 0.0f, 100.0f);

	//WVPデータを更新
	wvpResource_.back()->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_.back()));

	Matrix4x4 worldMatrix = MakeAffineMatrix(data.transform.scale, data.transform.rotate, data.transform.translate);
	wvpData_.back()->World = worldMatrix;
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	wvpData_.back()->WVP = worldViewProjectionMatrix;

	wvpResource_.back()->Unmap(0, nullptr);

	//マテリアルデータを更新
	materialResource_.back()->Map(0, nullptr, reinterpret_cast<void**>(&materialData_.back()));

	materialData_.back()->color = data.color;
	materialData_.back()->uvTransform = MakeAffineMatrix(data.uvTransform.scale, data.uvTransform.rotate, data.uvTransform.translate);
	materialData_.back()->enableLighting = false;

	materialResource_.back()->Unmap(0, nullptr);

	//Spriteの描画。変更が必要なものだけ変更する
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定
	commandList->IASetIndexBuffer(&indexBufferView_);	//IBVを設定
	commandList->SetGraphicsRootDescriptorTable(2, texture_->textureSrvHandleGPU());
	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_.back()->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_.back()->GetGPUVirtualAddress());
	//ドローコール
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Object_2D::Reset() {
	//使用するリソースの要素を空にしておく
	wvpResource_.clear();
	wvpData_.clear();
	materialResource_.clear();
	materialData_.clear();
}