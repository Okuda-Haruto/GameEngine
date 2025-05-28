#include "Object_3D.h"

#include <numbers>

#include "Matrix4x4_operation.h"
#include "Vector3_operation.h"

#include "CreateBufferResource.h"
#include "LoadObjFile.h"
#include "GetDescriptorHandle.h"
#include "LoadTexture.h"
#include "CreateTextureResource.h"
#include "UploadTextureData.h"

# pragma region Object_3D

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

	//Sprite用のインデックスリソースを作る
	indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * modelData_.vertices.size());

	//インデックスバッファビューを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックスは3つサイズ
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * modelData_.vertices.size());
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	//3角形2つを組合わせ4角形にする
	for (int i = 0; i < modelData_.vertices.size(); i++) {
		indexData_[i] = i;
	}

	indexResource_->Unmap(0, nullptr);

	//使用するリソースの要素を予め用意する
	wvpResource_.resize(kMaxIndex_);
	wvpData_.resize(kMaxIndex_);
	materialResource_.resize(kMaxIndex_);
	materialData_.resize(kMaxIndex_);

	//初期化
	for (int i = 0; i < kMaxIndex_; i++) {
		wvpResource_[i] = CreateBufferResource(device_, sizeof(TransformationMatrix));
		wvpData_[i] = nullptr;
		materialResource_[i] = CreateBufferResource(device_, sizeof(Material));
		materialData_[i] = nullptr;
	}

	//最初から始める
	index_ = 0;
}

void Object_3D::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Object_3D_Data& data) {

	assert(texture_ != nullptr);						//Textureがセットされていない場合止める
	assert(light_ != nullptr || isLighting_ == false);	//Lightがセットされていない場合止める

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

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定
	commandList->IASetIndexBuffer(&indexBufferView_);	//IBVを設定
	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList->SetGraphicsRootDescriptorTable(2, texture_->textureSrvHandleGPU());
	if (isLighting_) {
		commandList->SetGraphicsRootConstantBufferView(3, light_->directionalLightResource()->GetGPUVirtualAddress());	//Lighting
	}

	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_[index_]->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_[index_]->GetGPUVirtualAddress());
	//描画(DrawCall)
	commandList->DrawIndexedInstanced(UINT(modelData_.vertices.size()), 1, 0, 0, 0);

	//最大値を超えたら最初から
	index_++;
	if (index_ >= kMaxIndex_) {
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

# pragma endregion

# pragma region Sprite_3D

Sprite_3D::~Sprite_3D() {
	wvpResource_.clear();
	wvpData_.clear();
	materialResource_.clear();
	materialData_.clear();
	vertexData_ = nullptr;
}

void Sprite_3D::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t kWindowWidth, uint32_t kWindowHeight) {

	device_ = device;

	//頂点リソースを作る
	vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * 4);

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点は4つサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

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

	indexResource_->Unmap(0, nullptr);

	//使用するリソースの要素を予め用意する
	wvpResource_.resize(kMaxIndex_);
	wvpData_.resize(kMaxIndex_);
	materialResource_.resize(kMaxIndex_);
	materialData_.resize(kMaxIndex_);

	//初期化
	for (int i = 0; i < kMaxIndex_; i++) {
		wvpResource_[i] = CreateBufferResource(device_, sizeof(TransformationMatrix));
		wvpData_[i] = nullptr;
		materialResource_[i] = CreateBufferResource(device_, sizeof(Material));
		materialData_[i] = nullptr;
	}

	//最初から始める
	index_ = 0;
}

void Sprite_3D::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Object_3D_Data& data) {

	assert(texture_ != nullptr);						//Textureがセットされていない場合止める
	assert(light_ != nullptr || isLighting_ == true);	//Lightがセットされていない場合止める

	//頂点のローカル座標系を設定
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//左下
	vertexData_[0].position = { -spriteWidth_ / 2 / kWindowWidth_,-spriteHeight_ / 2 / kWindowHeight_,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[0].normal = { 0.0f,0.0f,-1.0f };
	//左上
	vertexData_[1].position = { -spriteWidth_ / 2 / kWindowWidth_,spriteHeight_ / 2 / kWindowHeight_,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[1].normal = { 0.0f,0.0f,-1.0f };
	//右下
	vertexData_[2].position = { spriteWidth_ / 2 / kWindowWidth_,-spriteHeight_ / 2 / kWindowHeight_,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[2].normal = { 0.0f,0.0f,-1.0f };
	//右上
	vertexData_[3].position = { spriteWidth_ / 2 / kWindowWidth_,spriteHeight_ / 2 / kWindowHeight_,0.0f,1.0f };
	vertexData_[3].texcoord = { 1.0f,0.0f };
	vertexData_[3].normal = { 0.0f,0.0f,-1.0f };

	vertexResource_->Unmap(0, nullptr);

	Matrix4x4 inverseMatrix = Inverse(data.camera.viewMatrix);

	//カメラからスプライトへの距離
	Vector3 CameraToSprite{
		data.transform.translate.x - inverseMatrix.m[3][0],
		0,
		data.transform.translate.z - inverseMatrix.m[3][2],
	};
	//正規化
	CameraToSprite = Normalize(CameraToSprite);

	Vector3 rotate{};
	rotate = data.transform.rotate;
	if (CameraToSprite.z > 0) {
		rotate.y = sinf(CameraToSprite.x) * std::numbers::pi_v<float> / 2;
	} else {
		rotate.y = sinf(CameraToSprite.x) * -std::numbers::pi_v<float> / 2 + std::numbers::pi_v<float>;
	}
	data.transform.rotate = rotate;

	//WVPデータを更新
	wvpResource_[index_]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_[index_]));

	Matrix4x4 worldMatrix = MakeAffineMatrix(data.transform.scale, rotate, data.transform.translate);
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

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定
	commandList->IASetIndexBuffer(&indexBufferView_);	//IBVを設定
	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList->SetGraphicsRootDescriptorTable(2, texture_->textureSrvHandleGPU());
	if (isLighting_) {
		commandList->SetGraphicsRootConstantBufferView(3, light_->directionalLightResource()->GetGPUVirtualAddress());	//Lighting
	}

	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_[index_]->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_[index_]->GetGPUVirtualAddress());
	//描画(DrawCall)
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

	//最大値を超えたら最初から
	index_++;
	if (index_ >= kMaxIndex_) {
		index_ = 0;
	}
}

void Sprite_3D::Reset() {
	//使用するリソースの要素を空にしておく
	wvpResource_.clear();
	wvpData_.clear();
	materialResource_.clear();
	materialData_.clear();

	//使用するリソースの要素を予め用意する
	wvpResource_.resize(kMaxIndex_);
	wvpData_.resize(kMaxIndex_);
	materialResource_.resize(kMaxIndex_);
	materialData_.resize(kMaxIndex_);

	//初期化
	for (int i = 0; i < kMaxIndex_; i++) {
		wvpResource_[i] = CreateBufferResource(device_, sizeof(TransformationMatrix));
		wvpData_[i] = nullptr;
		materialResource_[i] = CreateBufferResource(device_, sizeof(Material));
		materialData_[i] = nullptr;
	}

	//最初から始める
	index_ = 0;
}

# pragma endregion